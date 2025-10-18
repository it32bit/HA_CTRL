/**
 * @file Platform/STM32F4/Clock/clock_manager_stm32.cpp
 * @author it32bit
 * @brief Implements the ClockManager class for STM32F4 platform.
 *        Responsible for instantiating and initializing the platform-specific clock configuration.
 *
 * @version 1.0
 * @date 2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include "clock_manager_stm32.hpp"
#include "clock_stm32.hpp"
#include <new>

ClockManager::ClockManager() = default;

void ClockManager::initialize(void (*handler)())
{
    m_clock = new (m_storage) Clock_STM32F4();
    m_clock->configure(handler);
}
