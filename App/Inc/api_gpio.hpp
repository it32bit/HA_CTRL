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
constexpr size_t PIN_NAME_MAX_LENGTH   = 18;

extern const std::array<IOD, PIN_CONFIG_ARRAY_SIZE> ioPinConfigDefArray;

struct PinNameStruct
{
    char   name[PIN_NAME_MAX_LENGTH];
    size_t number;
};

/**
 * @brief Compile-time pin name to their corresponding index in ioPinConfigDefArray.
 *
 * This pair allows name-based access to GPIO configurations. Each string key (e.g. "LED_GREEN")
 * corresponds to an index in the ioPinConfigDefArray, which holds the actual pin setup.
 *
 * Example:
 * @code
 *      size_t index = boardPinNames.at("LED_RED");    // returns 2
 *      const IOD& pin = ioPinConfigDefArray[index];   // access GPIO definition
 *
 *      // In one line configuration to all pins.
 *      GpioManager<4> gpioMannager(ioPinConfigDefArray, pinLabelDefArray);
 * @endcode
 */
inline constexpr PinNameStruct pinLabelDefArray[PIN_CONFIG_ARRAY_SIZE] = {
    {"LED_GREEN",  0},
    {"LED_ORANGE", 1},
    {"LED_RED",    2},
    {"LED_BLUE",   3},
    {"BUTTON",     4},
    {"UART2_TX",   5},
    {"UART2_RX",   6}
};

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
    GpioDispatcher(const std::array<IOD, N>& t_io_defs,
                   const PinNameStruct (&t_pin_name_defs)[PIN_CONFIG_ARRAY_SIZE])
        : m_io_defs(t_io_defs), m_pin_name_array(t_pin_name_defs)
    {
    }

    PinController get(const char* t_pin_name) const
    {
        std::string_view pin_name{t_pin_name};
        for (const auto& entry : m_pin_name_array)
        {
            if (std::string_view(entry.name) == pin_name)
            {
                return PinController(m_io_defs[static_cast<int>(entry.number)]);
            }
        }

        for (;;)
        {
            /** Empty on purpose */
        }
    }

  private:
    const std::array<IOD, N>& m_io_defs;

    const PinNameStruct (&m_pin_name_array)[PIN_CONFIG_ARRAY_SIZE];
};

extern GpioDispatcher<PIN_CONFIG_ARRAY_SIZE> ioDispatcher;

#endif /* __API_GPIO_HPP */
