#include "gpio_pin_stm32.hpp"
#include "stm32f4xx_ll_gpio.h"

GpioPin_STM32::GpioPin_STM32(std::string_view name, GPIO_TypeDef* port, uint16_t pin)
    : m_port(port), m_pin_mask(1 << pin), m_name(name)
{
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

std::string_view GpioPin_STM32::name() const
{
    return m_name;
}
