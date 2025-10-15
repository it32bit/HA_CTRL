/**
 * @file gpio_pin_stm32.hpp
 * @author it32bit
 * @brief GPIO pin implementation for STM32F4 microcontrollers.
 * @version 0.1
 * @date 2025-10-15
 *
 */
#ifndef GPIO_PIN_STM32_HPP
#define GPIO_PIN_STM32_HPP

#include "pil_gpio.hpp"
#include "stm32f4xx_hal.h"
#include <string_view>

/**
 * @brief GPIO pin implementation for STM32F4 microcontrollers.
 *
 * This class provides methods to manipulate GPIO pins on STM32F4 series
 * microcontrollers. It implements the IGPIOPin interface.
 */
class GpioPin_STM32 : public IGPIOPin
{
  public:
    GpioPin_STM32(std::string_view name, GPIO_TypeDef* port, uint16_t pin);

    void toggle() override;
    void set() override;
    void reset() override;
    bool read() const override;

    std::string_view name() const override;

  private:
    GPIO_TypeDef*    m_port;
    uint16_t         m_pin_mask;
    std::string_view m_name;
};

#endif // GPIO_PIN_STM32_HPP
