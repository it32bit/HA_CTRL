/**
 ******************************************************************************
 * @file        : gpio_manager_stm32.cpp
 * @author      : i32bit
 * @brief       : Dispatcher for GPIO perypheral
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#include "gpio_pin_stm32.hpp"
#include "gpio_manager_stm32.hpp"
#include "gpio_hal_stm32.hpp"

void GpioManager::initialize(std::span<const PinConfig> t_configs)
{
    gpioHalConfig(t_configs);

    m_pinCount = std::min(t_configs.size(), PIN_CONFIG_ARRAY_SIZE);

    for (std::size_t i = 0; i < m_pinCount; ++i)
    {
        //static_assert(MAX_PORT_PINS_STM32 <= 16, "Too many pins for STM32 port");

        if (t_configs[i].pinNumber < MAX_PORT_PINS_STM32)
        {
            m_pins[i]       = GpioPin_STM32::createStatic(t_configs[i]);
            m_configsRef[i] = &t_configs[i];
        }
    }
}

IGPIOPin* GpioManager::getPin(std::string_view name)
{
    for (std::size_t i = 0; i < m_pinCount; ++i)
    {
        if (m_configsRef[i] && m_configsRef[i]->name == name)
        {

            return m_pins[i];
        }
    }
    return nullptr;
}
