/**
 ******************************************************************************
 * @file        gpio_hal_stm32.hpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-15
 * @brief       GPIO Hardware Abstraction Layer for STM32 microcontrollers.
 *
 *              Provides functions to configure GPIO pins using the standardized
 *              PinConfig structure. Enables platform-specific initialization
 *              while maintaining compatibility with the platform-independent layer (PIL).
 *
 * @copyright   Copyright (c) 2024–2025 it32bit
 * @license     MIT License
 *              This software is provided "as is", without warranty of any kind.
 ******************************************************************************
 */
#ifndef _GPIO_HAL_STM32_HPP_
#define _GPIO_HAL_STM32_HPP_

#include <cstddef>
#include <stdint-gcc.h>
#include <ranges>
#include <array>
#include <cassert>
#include "stm32f4xx_ll_gpio.h" // IWYU pragma: keep
#include "pil_pin_config.hpp"

/**
 * @brief Configure a GPIO pin based on the provided PinConfig structure.
 * This function sets up the GPIO pin according to the specified mode, pull-up/down,
 * speed, type, alternate function, and interrupt settings.
 * @param t_iodef The PinConfig structure containing the pin configuration.
 * @return true if the configuration was successful, false otherwise.
 * @note This function assumes that the necessary peripheral clocks are enabled.
 * @see PinConfig for details on the configuration options.
*/
extern bool gpioHalConfig(const PinConfig& t_iodef);

/**
 * @brief Configure multiple GPIO pins based on a range of PinConfig structures.
 * This function iterates over the provided range and applies the configuration
 * for each pin using the gpioHalConfig function.
 * @param t_iodef A range of PinConfig structures to configure.
 * @return true if all pins were configured successfully, false otherwise.
 * @note This function requires C++20 or later for range support.
 * @see https://en.cppreference.com/w/cpp/ranges for more information on ranges.
 *
 * For clarification this:
 * template <typename T>
 *      requires std::ranges::range<T>
 * bool gpioHalConfig(const T& t_iodef)
 *
 * Is the same as:
 * template <std::ranges::range Range> bool gpioHalConfig(const Range& t_iodef)
 */
template <std::ranges::range Range>
bool gpioHalConfig(const Range& t_iodef)
{
    for (auto& iopin : t_iodef)
    {
        gpioHalConfig(iopin);
    }
    return true;
}

/**
 * @brief Configure multiple GPIO pins based on a pair of iterators.
 * This function iterates from the begin iterator to the end iterator,
 * applying the configuration for each pin using the gpioHalConfig function.
 * @tparam Iter The type of the iterator.
 * @param begin The beginning iterator of the PinConfig range.
 * @param end The ending iterator of the PinConfig range.
 * @return true if all pins were configured successfully, false otherwise.
 * @note This function is useful for cases where a range is not available
 *       but iterators can be provided (e.g., from a container).
 */
template <typename Iter>
bool gpioHalConfig(Iter begin, Iter end)
{
    while (begin != end)
    {
        gpioHalConfig(*begin++);
    }
    return true;
}

/**
 * @brief Get the GPIO pin mask for a given pin number.
 * This function returns a bitmask corresponding to the specified pin number.
 * If the pin number is greater than HAL_GPIO_PIN_SHIFT_MAX, it returns 0.
 * @param pinNumber The pin number (0-15).
 * @return The GPIO pin mask as a uint32_t.
 */
constexpr std::size_t HAL_GPIO_PIN_SHIFT_MAX = 15;

[[nodiscard]] constexpr uint32_t getGpioPinMask(const uint_fast8_t t_pinNumber)
{
    assert(t_pinNumber <= HAL_GPIO_PIN_SHIFT_MAX);

    return (t_pinNumber > HAL_GPIO_PIN_SHIFT_MAX) ? 0U : (1U << t_pinNumber);
}

/**
 * @brief Array of GPIO port base addresses for STM32F4 series.
 * This array maps port indices to their corresponding GPIO_TypeDef pointers.
 * The order is based on the STM32F4 series GPIO ports: A, B, C, D, E, F, G, H, I.
 */
constexpr std::array<GPIO_TypeDef*, 9> gpioPortsStm32 = {GPIOA, GPIOB, GPIOC, GPIOD, GPIOE,
                                                         GPIOF, GPIOG, GPIOH, GPIOI};

/**
 * @brief Get the GPIO port base address from a port index.
 * This function returns the GPIO_TypeDef pointer corresponding to the given port index.
 * If the index is out of range, it returns nullptr.
 * @param t_index The port index (0 for GPIOA, 1 for GPIOB, etc.).
 * @return The GPIO_TypeDef pointer for the specified port, or nullptr if the index is invalid.
 */
[[nodiscard]] constexpr GPIO_TypeDef* getPortStm32FromIndex(uint8_t t_index) noexcept
{
    return t_index < gpioPortsStm32.size() ? gpioPortsStm32[t_index] : nullptr;
}

#endif // _GPIO_HAL_STM32_HPP_
