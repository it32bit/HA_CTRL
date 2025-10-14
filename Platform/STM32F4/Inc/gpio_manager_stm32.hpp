/**
 ******************************************************************************
 * @file        : gpio_manager_stm32.hpp
 * @author      : i32bit
 * @brief       : GPIO Manager for STM32 platform
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#ifndef _GPIO_MANAGER_STM32_HPP_
#define _GPIO_MANAGER_STM32_HPP_

#include "pil_gpio.hpp"
#include "pil_pin_config.hpp"
#include "gpio_config_stm32.hpp"
#include <vector>
#include <memory>
#include <span>

class GpioManager : public IGPIOManager
{
  public:
    void initialize(const std::span<const PinConfig> configs);

    IGPIOPin* getPin(std::string_view name) override;

  private:
    std::vector<std::unique_ptr<IGPIOPin>> m_pins;
};

#endif // _GPIO_MANAGER_STM32_HPP_
