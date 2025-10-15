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
#include "gpio_manager_stm32.hpp"
#include "gpio_pin_stm32.hpp"
#include "gpio_hal_stm32.hpp"

// Helper to convert port index to actual GPIOx pointer
static GPIO_TypeDef* portFromIndex(uint8_t index)
{
    static GPIO_TypeDef* ports[] = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE, GPIOF, GPIOG, GPIOH, GPIOI};
    if (index < sizeof(ports) / sizeof(ports[0]))
        return ports[index];
    return nullptr;
}

constexpr size_t MAX_PORT_PINS_STM32 = 16; // Arbitrary limit to avoid excessive memory usage

void GpioManager::initialize(const std::span<const PinConfig> t_configs)
{
    gpioHalConfig(t_configs);

    for (const auto& cfg : t_configs)
    {
        GPIO_TypeDef* port = portFromIndex(cfg.portIndex);
        if (port != nullptr && cfg.pinNumber < MAX_PORT_PINS_STM32)
        {
            m_pins.push_back(std::make_unique<GpioPin_STM32>(cfg.name, port, cfg.pinNumber));
        }
    }
}

IGPIOPin* GpioManager::getPin(std::string_view name)
{
    for (auto& pin : m_pins)
    {
        if (pin->name() == name)
            return pin.get();
    }
    return nullptr;
}
