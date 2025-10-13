#ifndef HAL_GPIO_HPP
#define HAL_GPIO_HPP

#include <cstddef>
#include <stdint-gcc.h>
#include <ranges>
#include "stm32f4xx_ll_gpio.h" // IWYU pragma: keep

/**
 * @brief Input / Output Definition
 */
using IOD = struct GPIOStuct
{
    GPIO_TypeDef* GPIO;

    uint32_t PinNb;

    enum MODER : uint32_t
    {
        INPUT  = 0,
        OUTPUT = 1,
        ALT    = 2,
        ANALOG = 3,
    } Moder;

    uint32_t AltFunc;

    enum TYPE : uint32_t
    {
        NORMAL    = 0,
        OPENDRAIN = 1
    } Type;

    enum SPEED : uint32_t
    {
        LOW      = 0,
        MEDIUM   = 1,
        HIGH     = 2,
        VERYHIGH = 3
    } Speed;

    enum PUPDR : uint32_t
    {
        NO       = 0,
        PULLUP   = 1,
        PULLDOWN = 2
    } Pupdr;

    enum GEXTI : uint32_t
    {
        NONE = 0,
        IT   = 1,
        EVT  = 2
    } Exti;

    enum ITRG : uint32_t
    {
        NOTRG    = 0,
        RISING   = 1,
        FALLING  = 2,
        RIS_FALL = 3
    } Trg;

    enum ISTATE : uint32_t
    {
        DONT_CARE,
        LOGIC_LOW,
        LOGIC_HIGH
    } InitState;

    uint32_t IPriority;
};

/**
 * @brief Computes the GPIO pin mask for a given pin number.
 *
 * This function returns a bitmask corresponding to the specified GPIO pin.
 * If the pin number is greater than 15, it returns 0.
 *
 * @param pinNumber GPIO pin number (0–15).
 *
 * @details Attribute [[nodiscard]] warns if return value is ignired.
 *          uint_fast8_t - for better performance
 *
 * @return Bitmask with the corresponding bit set, or 0 if out of range.
 */
constexpr std::size_t HAL_GPIO_PIN_SHIFT_MAX = 15;

[[nodiscard]] constexpr uint32_t getGpioPinMask(const uint_fast8_t pinNumber)
{
    return (pinNumber > HAL_GPIO_PIN_SHIFT_MAX) ? 0U : (1U << pinNumber);
}

/**
 * @brief Lower-level gpioConfig(def) function to handle individual GPIO setup.
 */
extern bool gpioConfig(const IOD& t_io);

/**
 * @section Template hal_gpio.hpp
 */

/**
 * @brief   template<typename Iter> bool gpioConfig
 * @param   Iter begin
 * @param   Iter end
 * @return  bool
 *
 * @attention Lower-level gpioConfig(def) function must exists to handle individual GPIO setup.
 *
 * @details Works with raw iterators. Functions provide flexible ways to batch-configure GPIOs
 *          This function takes a pair of iterators
 *          and configures each GPIO definition in the range [begin, end).
 *          It calls gpioConfig on each element individually.
 */
template <typename Iter>
bool gpioConfig(Iter begin, Iter end)
{
    while (begin != end)
    {
        gpioConfig(*begin++);
    }
    return true;
}

/**
 * @brief   template <typename T> requires std::ranges::range<T> bool gpioConfig(const T& iodef)
 * @param   T& iodef
 * @return  bool
 *
 * @attention Lower-level gpioConfig(def) function must exists to handle individual GPIO setup.
 *
 * @details Works with modern C++ ranges. Functions provide flexible ways to batch-configure GPIOs.
 *          Template accepts any range-compatible container (like std::vector, std::array, etc.)
 *          and iterates through it, configuring each GPIO definition using gpioConfig.
 */
template <typename T>
    requires std::ranges::range<T>
bool gpioConfig(const T& t_iodef)
{
    for (auto& iopin : t_iodef)
    {
        gpioConfig(iopin);
    }
    return true;
}

#endif // HAL_GPIO_HPP
