/**
 ******************************************************************************
 * @file        watchdog_manager_stm32.cpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-17
 * @brief       Watchdog manager for STM32 platform.
 *
 *              Provides a high-level interface for initializing and feeding the
 *              STM32 Independent Watchdog (IWDG). Internally constructs the watchdog
 *              instance using placement new and delegates feeding to the underlying
 *              Watchdog_STM32 implementation.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include <new>
#include <cstddef>  // for std::byte
#include "watchdog_manager_stm32.hpp"
#include "watchdog_stm32.hpp"

WatchdogManager::WatchdogManager() = default;

void WatchdogManager::initialize(uint32_t timeout_ms)
{
    // Construct the STM32 watchdog in place (inside m_storage)
    m_watchdog = new (m_storage) Watchdog_STM32(timeout_ms);

    feed();
}

void WatchdogManager::feed()
{
    if (m_watchdog)
    {
        m_watchdog->feed();
    }
}
