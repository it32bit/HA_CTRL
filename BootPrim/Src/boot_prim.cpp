/**
 * @file      BootPrim/Src/boot_prim.cpp
 * @author    it32bit
 * @brief     Primary bootloader stage for Platform (STM32F4, ESP32, etc.).
 *            Initializes system clock, manages flash-backed BootState flags,
 *            and jumps to the secondary bootloader (BootSec).
 *
 * @version   1.0
 * @date      2025-10-23
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include <cstdint>
#include "boot_prim.hpp"
#include "boot_flag_manager.hpp"
#include "stm32f4xx.h"
#include "clock_manager_stm32.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"
#include "firmware_metadata.hpp"
#include "crc32_check.hpp"
#include "image_manager.hpp"

namespace BootPrim
{

static bool checkMetaData(std::uintptr_t t_firmware, std::uintptr_t t_metadata);

static void JumpToBootSec()
{
    constexpr std::uintptr_t addr      = FlashLayout::BOOTLOADER2_START;
    uint32_t                 stack_ptr = *reinterpret_cast<volatile uint32_t*>(addr);
    uint32_t                 reset_ptr = *reinterpret_cast<volatile uint32_t*>(addr + 4);

    __disable_irq();
    SCB->VTOR = addr;
    __DSB();
    __ISB();
    __set_MSP(stack_ptr);

    auto entry = reinterpret_cast<void (*)()>(reset_ptr);
    entry();
}

ClockManager clock;

extern "C" int main()
{
    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);
    ImageManager       image(&writer);
    clock.initialize(nullptr);

    bool newBootSecCheck = checkMetaData(FlashLayout::NEW_BOOTLOADER2_START,
                                         FlashLayout::NEW_BOOTLOADER2_METADATA_START);
    if (newBootSecCheck == true)
    {
        image.writeImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::BOOTLOADER2_START,
                         FlashLayout::NEW_BOOTLOADER2_SIZE);
        image.clearImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);

        LEDControl::toggleBlueLED();
    }

    bool newAppCheck =
        checkMetaData(FlashLayout::NEW_APP_START, FlashLayout::NEW_APP_METADATA_START);
    auto isStaged = flags.getState();

    if (newAppCheck == true)
    {
        if (isStaged != BootState::Staged)
        {
            flags.setState(BootState::Staged);
        }

        LEDControl::toggleOrangeLED();
    }

    /**
     * Secend Bootloader Integrity check
     */
    bool bootSecCheck =
        checkMetaData(FlashLayout::BOOTLOADER2_START, FlashLayout::BOOT2_METADATA_START);

    if (bootSecCheck == true)
    {
        JumpToBootSec();
    }

    /**
     * Secend Bootloader Integrity check FAIL
     */
    uint32_t timer{};

    while (true)
    {
        /**
         * @brief Blinking period depends on number of uC cycles per loop (~30 - 50):
         *      Tc = 1/16MHz = 62.5ns,
         *      Tloop = Tc x CyclePerLoop,
         *      Ttoggle = Tloop x 1M,
         *  Obserwed blinking period ~1Hz
         */
        if ((timer % 1000000) == 0)
        {
            LEDControl::toggleOrangeLED();
        }
        ++timer;
    }
}

static bool checkMetaData(std::uintptr_t t_firmware, std::uintptr_t t_metadata)
{
    const std::uint8_t* firmware = reinterpret_cast<const std::uint8_t*>(t_firmware);

    const Firmware::Metadata* metadata = reinterpret_cast<const Firmware::Metadata*>(t_metadata);

    bool result = false;

    if (metadata->magic == Firmware::METADATA_MAGIC)
    {
        std::size_t   firmware_size = metadata->firmwareSize;
        std::uint32_t expected_crc  = metadata->firmwareCRC;

        result = Integrity::CRC32Checker::verify(firmware, firmware_size, expected_crc);
    }

    return result;
}

} // namespace BootPrim
