/**
 ******************************************************************************
 * @file        gpio_pin_stm32.cpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-15
 * @brief       GPIO pin control implementation for STM32 microcontrollers.
 *
 *              Implements the GpioPin_STM32 class, which provides basic GPIO operations
 *              such as toggle, set, reset, and read using the STM32 LL (Low-Layer) API.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#include "gpio_pin_stm32.hpp"
#include "gpio_config_stm32.hpp"
#include "gpio_stm32.hpp"
#include "stm32f4xx_ll_gpio.h"

void GpioPin_STM32::toggle()
{
    LL_GPIO_TogglePin(m_port, m_pin_mask);
}

void GpioPin_STM32::set()
{
    LL_GPIO_SetOutputPin(m_port, m_pin_mask);
}

void GpioPin_STM32::reset()
{
    LL_GPIO_ResetOutputPin(m_port, m_pin_mask);
}

bool GpioPin_STM32::read() const
{
    return (LL_GPIO_ReadInputPort(m_port) & m_pin_mask) != 0;
}
