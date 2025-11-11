/**
 ******************************************************************************
 * @file        clock_boot_prim_stm32.cpp
 * @author      it32bit
 * @brief       Implements the ClockBootPrim_STM32F4 class for configuring the Primary Bootloader clock on STM32F4 devices.
 *              Supports optional error handling via user-provided callback.
 *
 * @version     1.0
 * @date        2025-10-19
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include "clock_boot_prim_stm32.hpp"
#include "stm32f4xx_hal.h"

void Clock_BootPrim::configure(void (*handler)())
{
    // 1. Disable watchdog (IWDG)
    if ((IWDG->SR & IWDG_SR_PVU) == 0)
    {
        IWDG->KR  = 0x5555; // Enable access
        IWDG->PR  = 0x00;   // Prescaler
        IWDG->RLR = 0x0FFF;
        IWDG->KR  = 0xAAAA; // Reload
        IWDG->KR  = 0x0000; // Attempt to stop (not always possible if started by option bytes)
    }

    // 2. Use HSI (default after reset)
    RCC->CR |= RCC_CR_HSION;
    while ((RCC->CR & RCC_CR_HSIRDY) == 0)
    {
    }

    RCC->CFGR = 0x00000000; // Reset clock config
    RCC->CIR  = 0x00000000; // Disable all clock interrupts

    // 3. Optional: call error handler if needed
    if (handler)
    {
        handler();
    }
}
