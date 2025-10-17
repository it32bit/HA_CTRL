/**
 ******************************************************************************
 * @file        watchdog_manager.hpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-17
 * @brief       High-level watchdog manager interface for STM32 platform.
 *
 *              Declares the WatchdogManager class, which encapsulates watchdog
 *              initialization and feeding logic. Internally constructs a platform-specific
 *              watchdog instance using placement new and exposes a uniform interface
 *              via the IWatchdog abstraction.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef WATCHDOG_MANAGER_HPP
#define WATCHDOG_MANAGER_HPP

#include <stdint-gcc.h>
#include <cstdint>
#include <cstddef>
#include "pil_watchdog.hpp"

class WatchdogManager
{
public:
    WatchdogManager();

    void initialize(uint32_t timeout_ms);
    void feed();

private:
    static constexpr std::size_t MaxWatchdogSize = sizeof(uint32_t) * 8; // enough for Watchdog_STM32
    static constexpr std::size_t MaxWatchdogAlign = alignof(uint32_t);

    alignas(MaxWatchdogAlign) std::byte m_storage[MaxWatchdogSize];
    IWatchdog* m_watchdog = nullptr;
};

#endif // WATCHDOG_MANAGER_HPP
