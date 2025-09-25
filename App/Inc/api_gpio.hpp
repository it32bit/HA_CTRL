/**
 ******************************************************************************
 * @file        : api_gpio.hpp
 * @author      : i32bit
 * @brief       : API for GPIO perypheral
 ******************************************************************************
 */
#ifndef __API_GPIO_HPP
#define __API_GPIO_HPP

#include "hal_gpio.hpp"
#include <map>

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
[[nodiscard]] static constexpr uint32_t getGpioPinMask(const uint_fast8_t pinNumber)
{
    return (pinNumber > 15u) ? 0u : (1u << pinNumber);
}

/**
 * Simple PIN controller class
 */
class PinController
{
  public:
    PinController(const IOD& io) : ioPort(io.GPIO) { ioPin = getGpioPinMask(io.PinNb); }

    PinController(GPIO_TypeDef* port, uint16_t pin) : ioPort(port) { ioPin = getGpioPinMask(pin); }

    void toggle() const { LL_GPIO_TogglePin(ioPort, ioPin); }

    void on() { LL_GPIO_SetOutputPin(ioPort, ioPin); }

    void off() { LL_GPIO_ResetOutputPin(ioPort, ioPin); }

  private:
    GPIO_TypeDef* ioPort;
    uint32_t      ioPin;
};

template <size_t N> class GpioManager
{
  public:
    GpioManager(const std::array<IOD, N>& ioDefs, const std::map<std::string, size_t>& nameMap)
        : pinArrayDef(ioDefs), nMap(nameMap)
    {
    }

    PinController get(const std::string& name) const
    {
        auto it = nMap.find(name);
        if (it != nMap.end() && it->second <= N)
        {
            return PinController(pinArrayDef[it->second]);
        }
        else
        {
            for (;;)
            {
                /** Empty on purpose */
            }
        }
    }

  private:
    const std::array<IOD, N>& pinArrayDef;

    const std::map<std::string, size_t> nMap;
};

#endif /* __API_GPIO_HPP */