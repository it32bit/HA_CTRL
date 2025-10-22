// boot_prim.cpp
/**
 * BootPrim → BootSec → Application
 * Flash-backed boot flags using BootState
 * Direct flash access via your IFlashWriter interface
 */

#include <cstdint>
#include "stm32f4xx.h"

#include "boot_flag_manager.hpp"
#include "flash_writer_stm32.hpp"
#include "flash_layout.hpp"
/**
 * Embed in binary section best for: Production firmware
 */
__attribute__((section(".boot_flag_init"))) const std::uint32_t boot_flag_default =
    0x53544147; // 'STAG'

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

extern "C" int main()
{
    FlashWriterSTM32F4 writer;
    BootFlagManager    flags(&writer);

    /**
     * Flash-backed boot flags using BootState
     */
    if (flags.getState() == BootState::Staged)
    {
        JumpToBootSec();
    }

    while (true)
    {
    }
}

} // namespace BootPrim
