#include "gpio_pin_stm32.hpp"
#include "stm32f4xx_ll_gpio.h"

GpioPin_STM32::GpioPin_STM32(GPIO_TypeDef* port, uint16_t pin, std::string_view pinName)
    : m_port(port), m_pin(pin), m_name(pinName)
{
}

void GpioPin_STM32::toggle()
{
    LL_GPIO_TogglePin(m_port, m_pin);
}

void GpioPin_STM32::set()
{
    LL_GPIO_SetOutputPin(m_port, m_pin);
}

void GpioPin_STM32::reset()
{
    LL_GPIO_ResetOutputPin(m_port, m_pin);
}

bool GpioPin_STM32::read() const
{
    return (LL_GPIO_ReadInputPort(m_port) & m_pin) != 0;
}
