/**
 * @file      BootPrim/Src/boot_prim.cpp
 * @author    it32bit
 * @brief     Primary bootloader stage for Platform (STM32F4, ESP32, etc.).
 *            Initializes system clock, manages flash-backed BootState flags,
 *            and jumps to the secondary bootloader (BootSec).
 * @note      Flash-backed boot flags using BootState
 *            Direct flash access via your IFlashWriter interface
 *
 * @version   1.0
 * @date      2025-10-23
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include <cstdint>
#include "stm32f4xx.h"
#include "boot_prim.hpp"
#include "boot_flag_manager.hpp"
#include "clock_manager_stm32.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"

namespace BootPrim
{

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
    uint32_t           timer{};

    clock.initialize(nullptr);

    auto isStaged = flags.getState();

    if (isStaged != BootState::Staged && isStaged != BootState::Verified &&
        isStaged != BootState::Applied)
    {
        flags.setState(BootState::Staged);
        LEDControl::toggleOrangeLED();
    }

    /**
     * Flash-backed boot flags using BootState
     */
    isStaged = flags.getState();

    if (isStaged == BootState::Staged)
    {
        JumpToBootSec();
    }

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
