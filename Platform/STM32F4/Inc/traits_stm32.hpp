/**
 * @file traits_stm32.hpp
 * @author it32bit
 * @brief GPIO Hardware Abstraction Layer traits for STM32 microcontrollers.
 *        Maps generic PIL interfaces to STM32-specific implementations,
 *        enabling compile-time type resolution.
 *
 * @version 1.0
 * @date 2025-10-15
 * @copyright Copyright (c) 2024–2025 it32bit
 * @license MIT License
 */
#ifndef TRAITS_STM32_HPP
#define TRAITS_STM32_HPP

#include "gpio_hal_stm32.hpp"
#include "gpio_pin_stm32.hpp"
#include "gpio_manager_stm32.hpp"

struct STM32Traits
{
    using PinType     = GpioPin_STM32;
    using ManagerType = GpioManager;

    // Provide a function to return port base from index
    static GPIO_TypeDef* portFromIndex(uint8_t idx) { return getPortStm32FromIndex(idx); }

    // Provide other mappings needed (e.g. clock enable)
};

#endif