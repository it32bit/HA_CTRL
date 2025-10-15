/**
 * @file gpio_pin_stm32.cpp
 * @author it32bit
 * @brief GPIO Pin implementation for STM32 microcontrollers.
 * This file provides the implementation of the GpioPin_STM32 class,
 * which encapsulates GPIO pin operations such as toggle, set, reset, and read.
 * @version 1.0
 * @date 2025-10-15
 */
#include "gpio_pin_stm32.hpp"
#include "gpio_config_stm32.hpp"
#include "gpio_hal_stm32.hpp"
#include "stm32f4xx_ll_gpio.h"



GpioPin_STM32* GpioPin_STM32::createStatic(const PinConfig& t_cfg)
{
    static std::array<GpioPin_STM32, PIN_CONFIG_ARRAY_SIZE> pool{};
    static std::size_t                                      next = 0;

    if (next >= PIN_CONFIG_ARRAY_SIZE)
    {
        return nullptr;
    }

    GPIO_TypeDef* port = getPortStm32FromIndex(t_cfg.portIndex);
    if (port == nullptr || t_cfg.pinNumber > 15)
    {
        return nullptr;
    }

    gpioHalConfig(t_cfg);

    pool[next] = GpioPin_STM32(t_cfg.name, port, t_cfg.pinNumber);

    return &pool[next++];
}

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
