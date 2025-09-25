/**
 ******************************************************************************
 * @file        : api_gpio.hpp
 * @author      : i32bit
 * @brief       : API for GPIO perypheral
 ******************************************************************************
 * This software is licensed under the MIT License.
 * Provided "as is", without warranty of any kind.
 * The author is not liable for any damages resulting from its use.
 ******************************************************************************
 */
#ifndef __API_GPIO_HPP
#define __API_GPIO_HPP

#include "hal_gpio.hpp"
#include <string_view>

constexpr size_t PIN_CONFIG_ARRAY_SIZE = 5;

extern const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray;
extern const std::array<std::pair<std::string_view, size_t>, PIN_CONFIG_ARRAY_SIZE>
    pinLabelDefArray;


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

template <size_t N> class GpioDispatcher
{
  public:
    GpioDispatcher(const std::array<IOD, N>&                                 ioDefs,
                   const std::array<std::pair<std::string_view, size_t>, N>& nameDefs)
        : ioDefCfg(ioDefs), pinLabelArray(nameDefs)
    {
    }

    PinController get(std::string_view pinLabel) const
    {
        for (const auto& entry : pinLabelArray)
        {
            if (entry.first == pinLabel)
            {
                return PinController(ioDefCfg[static_cast<int>(entry.second)]);
            }
        }

        for (;;)
        {
            /** Empty on purpose */
        }
    }

  private:
    const std::array<IOD, N>&                                 ioDefCfg;
    const std::array<std::pair<std::string_view, size_t>, N>& pinLabelArray;
};

#endif /* __API_GPIO_HPP */
