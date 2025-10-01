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

#include <string_view>
#include "hal_gpio.hpp"

constexpr size_t PIN_CONFIG_ARRAY_SIZE = 7;

extern const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray;
extern const std::array<std::pair<std::string_view, size_t>, PIN_CONFIG_ARRAY_SIZE>
    pinLabelDefArray;

/**
 * Simple PIN controller class
 */
class PinController
{
  public:
    PinController(const IOD& t_io) : m_io_port(t_io.GPIO)
    {
        m_io_pin_mask = getGpioPinMask(t_io.PinNb);
    }

    PinController(GPIO_TypeDef* t_port, uint16_t t_pin) : m_io_port(t_port)
    {
        m_io_pin_mask = getGpioPinMask(t_pin);
    }

    void toggle() const { LL_GPIO_TogglePin(m_io_port, m_io_pin_mask); }

    void on() const { LL_GPIO_SetOutputPin(m_io_port, m_io_pin_mask); }

    void off() const { LL_GPIO_ResetOutputPin(m_io_port, m_io_pin_mask); }

    bool readInputPin() const
    {
        uint32_t ioInput = uint32_t(LL_GPIO_ReadInputPort(m_io_port) & m_io_pin_mask);
        return static_cast<bool>(ioInput);
    }

  private:
    GPIO_TypeDef* m_io_port;
    uint32_t      m_io_pin_mask;
};

template <size_t N>
class GpioDispatcher
{
  public:
    GpioDispatcher(const std::array<IOD, N>&                                 t_io_defs,
                   const std::array<std::pair<std::string_view, size_t>, N>& t_pin_name_defs)
        : m_io_defs(t_io_defs), m_pin_name_array(t_pin_name_defs)
    {
    }

    PinController get(std::string_view t_pin_name) const
    {
        for (const auto& entry : m_pin_name_array)
        {
            if (entry.first == t_pin_name)
            {
                return PinController(m_io_defs[static_cast<int>(entry.second)]);
            }
        }

        for (;;)
        {
            /** Empty on purpose */
        }
    }

  private:
    const std::array<IOD, N>&                                 m_io_defs;
    const std::array<std::pair<std::string_view, size_t>, N>& m_pin_name_array;
};

extern GpioDispatcher<PIN_CONFIG_ARRAY_SIZE> ioDispatcher;

#endif /* __API_GPIO_HPP */
