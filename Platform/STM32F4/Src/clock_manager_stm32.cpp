/**
 ******************************************************************************
 * @file        clock_manager_stm32.cpp
 * @author      it32bit
 * @brief       Implements the ClockManager class for the STM32F4 platform.
 *              Dynamically selects and initializes the appropriate clock configuration
 *              based on the build context (e.g., BootPrim vs App/BootSec).
 *
 * @version     1.0
 * @date        2025-10-19
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include "clock_manager_stm32.hpp"
#include <new>

#ifdef BOOT_PRIM
    /**
     * @brief Clock configuration for Primary Bootloader.
     * @note  Uses minimal setup (HSI only) with no HAL dependencies.
     *        - Compile-time selection (no runtime branching)
     *        - No manual injection required
     *        - Easily extendable for other platforms
     */
    #include "clock_boot_prim_stm32.hpp"
#else
    /**
     * @brief Clock configuration for Application and Secondary Bootloader.
     * @note  Uses full-featured Clock_STM32F4 implementation.
     */
    #include "clock_stm32.hpp"
#endif

ClockManager::ClockManager() = default;

void ClockManager::initialize(void (*handler)())
{
#ifdef BOOT_PRIM
    m_clock = new (m_storage) Clock_BootPrim();
#else
    m_clock = new (m_storage) Clock_STM32F4();
#endif
    if (m_clock)
    {
        m_clock->configure(handler);
    }
}
