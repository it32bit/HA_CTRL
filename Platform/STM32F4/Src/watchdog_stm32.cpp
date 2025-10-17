/**
 ******************************************************************************
 * @file        watchdog_stm32.cpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-17
 * @brief       Independent Watchdog (IWDG) driver for STM32 microcontrollers.
 *
 *              Implements the Watchdog_STM32 class, which configures and manages
 *              the IWDG peripheral using the STM32 LL (Low-Layer) API. It calculates
 *              the reload value based on the desired timeout and ensures proper
 *              initialization and feeding of the watchdog.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include "watchdog_stm32.hpp"
#include "stm32f4xx_ll_iwdg.h"

Watchdog_STM32::Watchdog_STM32(uint32_t t_timeout_ms)
{
    constexpr uint32_t prescaler    = 64;
    constexpr uint32_t lsi_freq     = 32000;
    const uint32_t     tick_time_ms = (prescaler * 1000) / lsi_freq;

    m_reload_value = t_timeout_ms / tick_time_ms;
    if (m_reload_value > 0x0FFF)
        m_reload_value = 0x0FFF;

    LL_IWDG_Enable(IWDG);
    LL_IWDG_EnableWriteAccess(IWDG);
    LL_IWDG_SetPrescaler(IWDG, LL_IWDG_PRESCALER_64);
    LL_IWDG_SetReloadCounter(IWDG, m_reload_value);

    waitReady();
    feed();
}

void Watchdog_STM32::feed()
{
    LL_IWDG_ReloadCounter(IWDG);
}

void Watchdog_STM32::waitReady()
{
    while (LL_IWDG_IsReady(IWDG) == 0)
    {
        // wait
    }
}
