/**
 * @file gpio_pin_stm32.hpp
 * @author it32bit
 * @brief GPIO pin implementation for STM32F4 microcontrollers.
 * @version 0.1
 * @date 2025-10-15
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef GPIO_PIN_STM32_HPP
#define GPIO_PIN_STM32_HPP

#include "pil_gpio.hpp"
#include "pil_pin_config.hpp"
#include "stm32f4xx_hal.h"
#include <string_view>

/**
 * @brief GPIO pin implementation for STM32F4 microcontrollers.
 * This class provides methods to control a GPIO pin (set, reset, toggle, read)
 * using the STM32 HAL library. It implements the IGPIOPin interface.
 * @note This class does not manage the pin configuration (mode, pull-up/down, speed, etc.).
 *       Pin configuration should be done separately using the PinConfig structure
 *       and appropriate HAL functions.
 * @see IGPIOPin for the interface definition.
 * @see PinConfig for pin configuration options.

 */
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
