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

constexpr size_t MAX_PORT_PINS_STM32 = 16; // Arbitrary limit to avoid excessive memory usage

class GpioManager : public IGPIOManager
{
  public:
    constexpr GpioManager() = default;

    void initialize(const std::span<const PinConfig> t_configs);

    [[nodiscard]] IGPIOPin* getPin(std::string_view t_name) override;

    void reset()
    {
        m_pins.fill(nullptr);
        m_configsRef.fill(nullptr);
        m_pinCount = 0;
    }

  private:
    std::array<GpioPin_STM32, PIN_CONFIG_ARRAY_SIZE>    m_pinPool{};
    std::array<IGPIOPin*, PIN_CONFIG_ARRAY_SIZE>        m_pins{};
    std::array<const PinConfig*, PIN_CONFIG_ARRAY_SIZE> m_configsRef{};
    std::size_t                                         m_pinCount = 0;
};

#endif // _GPIO_MANAGER_STM32_HPP_
