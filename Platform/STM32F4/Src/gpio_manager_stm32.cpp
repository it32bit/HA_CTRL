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
#include "IOD.hpp" 

GpioManager_STM32::GpioManager_STM32() {}

void GpioManager_STM32::initializePins(const std::vector<IOD>& pinConfigs)
{
    for (const auto& cfg : pinConfigs)
    {
        // Create pin wrappers, store them
        m_pins.emplace_back(
            std::make_unique<GpioPin_STM32>(cfg.GPIO, getGpioPinMask(cfg.PinNb), /*name*/ ""));
    }
}

IGPIOPin* GpioManager_STM32::getPin(std::string_view pinName)
{
    for (auto& pin : m_pins)
    {
        if (pin->name() == pinName)
            return pin.get();
    }
    return nullptr;
}
