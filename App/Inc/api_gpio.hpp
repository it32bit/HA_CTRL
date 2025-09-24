/**
 ******************************************************************************
 * @file           :
 ******************************************************************************
 */
#ifndef __API_GPIO_HPP
#define __API_GPIO_HPP

#include "hal_gpio.hpp"
#include <map>

static constexpr uint32_t pinMask(const uint32_t PinNb)
{
    if (PinNb > 15u)
        return 0u;
    return (uint32_t)1u << (PinNb);
}

/**
 * Simple PIN controller class
 */
class PinController
{
  public:
    PinController(const IOD& io) : ioport(io.GPIO) { iopin = pinMask(io.PinNb); }

    PinController(GPIO_TypeDef* port, uint16_t pin) : ioport(port) { iopin = pinMask(pin); }

    void toggle() { LL_GPIO_TogglePin(ioport, iopin); }

    void on() { LL_GPIO_SetOutputPin(ioport, iopin); }

    void off() { LL_GPIO_ResetOutputPin(ioport, iopin); }

  private:
    GPIO_TypeDef* ioport;
    uint32_t      iopin;
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
        //if (it != nMap.end() && it->second <= N)
        {
            return PinController(pinArrayDef[it->second]);
        }
        //throw std::runtime_error("Invalid GPIO name");
    }

  private:
    const std::array<IOD, N>&           pinArrayDef;
    const std::map<std::string, size_t> nMap;
};

#endif /* __API_GPIO_HPP */