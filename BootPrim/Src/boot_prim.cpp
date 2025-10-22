// boot_prim.cpp
/**
 * BootPrim → BootSec → Application
 * Flash-backed boot flags using BootState
 * Direct flash access via your IFlashWriter interface
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

    if ((flags.getState() == BootState::BLANK) || (flags.getState() == BootState::Idle))
    {
        // ISSUE-1: Not able to write and read if valid
        // flags.setState(BootState::Staged);
        JumpToBootSec();
    }

    auto isStaged = flags.getState();
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
