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
    reset(); // Clear existing state

    m_pinCount = std::min(t_configs.size(), PIN_CONFIG_ARRAY_SIZE);

    for (std::size_t i = 0; i < m_pinCount; ++i)
    {
        const auto& cfg = t_configs[i];

        // Validate pin number
        if (cfg.pinNumber >= MAX_PORT_PINS_STM32)
            continue;

        // Validate port
        GPIO_TypeDef* port = getPortStm32FromIndex(cfg.portIndex);
        if (!port)
            continue;

        // Configure pin via HAL
        if (!gpioHalConfig(cfg))
            continue;

        // Statically construct the pin (assign into pre-allocated array)
        m_pinPool[i] = GpioPin_STM32(cfg.name, port, cfg.pinNumber);

        m_pins[i]       = &m_pinPool[i];
        m_configsRef[i] = &cfg;
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
