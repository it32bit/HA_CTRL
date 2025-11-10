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
#include "clock_manager_stm32.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"
#include "image_manager.hpp"

namespace BootPrim
{

ClockManager clock;

extern "C" int main()
{
    clock.initialize(nullptr);

    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);
    ImageManager       image(&writer);

    /**
     * Boot-Sec
     */
    bool newBootSecCandidateCheck = isImageAuthentic(FlashLayout::NEW_BOOTLOADER2_START,
                                                     FlashLayout::NEW_BOOTLOADER2_METADATA_START);

    bool newBootSecCandidateDiffrent = isImageDiffrent(FlashLayout::BOOT2_METADATA_START,
                                                       FlashLayout::NEW_BOOTLOADER2_METADATA_START);
    if (newBootSecCandidateCheck == true)
    {
        if (newBootSecCandidateDiffrent == true)
        {
            image.writeImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::BOOTLOADER2_START,
                             FlashLayout::NEW_BOOTLOADER2_SIZE);

            LEDControl::toggleBlueLED();
        }

        image.clearImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);
    }
    else
    {
        bool newBootSecCandidateFirmwareEmpty =
            isImageEmpty(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);
        bool newBootSecCandidateMetaEmpty =
            isImageEmpty(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);
        if ((newBootSecCandidateFirmwareEmpty == false) || (newBootSecCandidateMetaEmpty == false))
        {
            image.clearImage(FlashLayout::NEW_BOOTLOADER2_START, FlashLayout::NEW_BOOTLOADER2_SIZE);
        }
    }

    /**
     * Application
     */
    bool newAppCheck = isImageStaged(FlashLayout::NEW_APP_METADATA_START);

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
        isImageAuthentic(FlashLayout::BOOTLOADER2_START, FlashLayout::BOOT2_METADATA_START);

    if (bootSecCheck == true)
    {
        Bootloader::jumpToAddress(FlashLayout::BOOTLOADER2_START);
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

} // namespace BootPrim
