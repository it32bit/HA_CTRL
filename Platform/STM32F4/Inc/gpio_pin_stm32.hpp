/**
 ******************************************************************************
 * @file        gpio_pin_stm32.hpp
 * @author      it32bit
 * @version     0.1
 * @date        2025-10-15
 * @brief       GPIO pin control implementation for STM32F4 microcontrollers.
 *
 *              Provides methods to manipulate GPIO pins (set, reset, toggle, read).
 *              Implements the IGPIOPin interface.
 *
 * @note        This class does not handle pin configuration (mode, pull, speed, etc.).
 *              Configuration should be performed separately using the PinConfig structure
 *              and platform-specific HAL setup functions.
 *
 * @see         IGPIOPin for interface definition.
 * @see         PinConfig for configuration structure.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef GPIO_PIN_STM32_HPP
#define GPIO_PIN_STM32_HPP

#include "pil_gpio.hpp"
#include "pil_pin_config.hpp"
#include "stm32f4xx_hal.h"
#include <string_view>


class GpioPin_STM32 : public IGPIOPin
{
  public:
    constexpr GpioPin_STM32() = default;

    constexpr GpioPin_STM32(GPIO_TypeDef* t_port, uint16_t t_pin)
        : m_port(t_port), m_pin_mask(get16BitMask(t_pin))
    {
    }

    void toggle() override;
    void set() override;
    void reset() override;
    bool read() const override;

  private:
    GPIO_TypeDef* m_port = nullptr;
    uint32_t      m_pin_mask{};

    [[nodiscard]]
    constexpr uint32_t get16BitMask(uint_fast8_t pinNumber)
    {
        return (pinNumber > 15) ? 0U : (1U << pinNumber);
    }
};

#endif // GPIO_PIN_STM32_HPP
