// boot_prim.cpp
#include <cstdint>
#include "stm32f4xx.h"
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

    while (true)
    {
    }
}

extern "C" int main()
{
    HAL_Init(); // Optional
    JumpToBootSec();
    while (true)
    {
    }
}

} // namespace BootPrim
