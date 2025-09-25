#ifndef HAL_GPIO_HPP
#define HAL_GPIO_HPP

#include <array>
#include <ranges>
#include "stm32f4xx.h"
#include "stm32f4xx_ll_gpio.h"

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
 *          uint_fast8_t - for better perfor
 *
 * @return Bitmask with the corresponding bit set, or 0 if out of range.
 */
    [[nodiscard]] constexpr uint32_t getGpioPinMask(const uint_fast8_t pinNumber){
        return (pinNumber > 15u) ? 0u : (1u << pinNumber);}

/**
 * @brief Lower-level hal_ConfigGpio(def) function to handle individual GPIO setup.
 */
bool hal_ConfigGpio(const IOD& def);

/**
 * @section Template hal_gpio.hpp
 */

/**
 * @brief   template<typename Iter> bool hal_ConfigGpio
 * @param   Iter begin
 * @param   Iter end
 * @return  bool
 *
 * @attention Lower-level hal_ConfigGpio(def) function must exists to handle individual GPIO setup.
 *
 * @details Works with raw iterators. Functions provide flexible ways to batch-configure GPIOs
 *          This function takes a pair of iterators
 *          and configures each GPIO definition in the range [begin, end).
 *          It calls hal_ConfigGpio on each element individually.
 */
template <typename Iter> bool hal_ConfigGpio(Iter begin, Iter end)
{
    while (begin != end)
    {
        hal_ConfigGpio(*begin++);
    }
    return true;
}

/**
 * @brief   template <typename T> requires std::ranges::range<T> bool hal_ConfigGpio(const T& iodef)
 * @param   T& iodef
 * @return  bool
 *
 * @attention Lower-level hal_ConfigGpio(def) function must exists to handle individual GPIO setup.
 *
 * @details Works with modern C++ ranges. Functions provide flexible ways to batch-configure GPIOs.
 *          Template accepts any range-compatible container (like std::vector, std::array, etc.)
 *          and iterates through it, configuring each GPIO definition using hal_ConfigGpio.
 */
template <typename T>
    requires std::ranges::range<T>
bool hal_ConfigGpio(const T& iodef)
{
    for (auto& def : iodef)
    {
        hal_ConfigGpio(def);
    }
    return true;
}

#endif // HAL_GPIO_HPP
