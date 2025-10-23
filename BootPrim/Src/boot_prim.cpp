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
#include "boot_flag_manager.hpp"
#include "clock_manager_stm32.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"

/**
 * Another option is to set in main() on first boot best for: Development
 *  if (flags.getState() != BootState::Staged &&
 *      flags.getState() != BootState::Verified &&
 *      flags.getState() != BootState::Applied)
 *      {
 *          flags.setState(BootState::Staged);
 *      }
 */
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

    clock.initialize(nullptr);

    auto isStaged = flags.getState();

    if ((isStaged == BootState::BLANK) || (isStaged == BootState::Idle))
    {
        // TODO: ISSUE-1: BootState - Not able to write and validate
        flags.setState(BootState::Staged);
        JumpToBootSec();
    }

    isStaged = BootState::Staged; // TODO: to be removed when ISSUE-1 will be Fixed

    /**
     * Flash-backed boot flags using BootState
     */
    if (isStaged == BootState::Staged)
    {
        JumpToBootSec();
    }

    while (true)
    {
    }
}

} // namespace BootPrim
