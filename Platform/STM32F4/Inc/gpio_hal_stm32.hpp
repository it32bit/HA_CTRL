#ifndef _GPIO_HAL_STM32_HPP_
#define _GPIO_HAL_STM32_HPP_

#include <cstddef>
#include <stdint-gcc.h>
#include <ranges>
#include "stm32f4xx_ll_gpio.h" // IWYU pragma: keep
#include "pil_pin_config.hpp"

constexpr std::size_t HAL_GPIO_PIN_SHIFT_MAX = 15;

[[nodiscard]]
constexpr uint32_t getGpioPinMask(const uint_fast8_t pinNumber)
{
    return (pinNumber > HAL_GPIO_PIN_SHIFT_MAX) ? 0U : (1U << pinNumber);
}

extern bool gpioHalConfig(const PinConfig& t_iodef);

template <typename T>
    requires std::ranges::range<T>
bool gpioHalConfig(const T& t_iodef)
{
    for (auto& iopin : t_iodef)
    {
        gpioHalConfig(iopin);
    }
    return true;
}

template <typename Iter>
bool gpioHalConfig(Iter begin, Iter end)
{
    while (begin != end)
    {
        gpioHalConfig(*begin++);
    }
    return true;
}

#endif // _GPIO_HAL_STM32_HPP_
