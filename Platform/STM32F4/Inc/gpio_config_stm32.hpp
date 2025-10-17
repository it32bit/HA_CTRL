/**
 ******************************************************************************
 * @file        gpio_config_stm32.hpp
 * @author      it32bit
 * @brief       GPIO configuration and label mapping for board-level initialization.
 *
 *              Provides compile-time configuration data for GPIO setup and pin labeling.
 *              Enables platform-agnostic initialization using static arrays, without runtime parsing.
 *
 * @details     The `gpioPinConfigs` array defines hardware parameters and symbolic labels for each pin.
 *              This allows both the Application and Bootloader to configure GPIOs in a unified way,
 *              without including platform-specific headers or relying on dynamic allocation.
 *
 * @usage
 * @code
 *  #include "gpio_manager_stm32.hpp"
 *
 *  GpioManager gpio;
 *  gpio.initialize(gpioPinConfigs);
 *  auto led = gpio.getPin(PinId::LD_RED);
 *
 *  led->set();
 * @endcode
 ******************************************************************************
 */
#ifndef GPIO_CONFIG_STM32_HPP
#define GPIO_CONFIG_STM32_HPP

#include <array>
#include <string_view>
#include "pil_pin_config.hpp"
#include "gpio_pin_stm32.hpp"
#include "pil_pin_id.hpp"

using enum PinConfig::Mode;
using enum PinConfig::Pull;
using enum PinConfig::Type;
using enum PinConfig::Speed;
using enum PinConfig::InterruptTrigger;
using enum PinConfig::PortStm32;
using enum PinConfig::InterruptExti;

constexpr std::size_t PIN_CONFIG_ARRAY_SIZE = static_cast<std::size_t>(PinId::COUNT);

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */
constexpr std::array<PinConfig, PIN_CONFIG_ARRAY_SIZE> gpioPinConfigs = {
    {{PinId::BUTTON, PortA, 0, Input, PullDown, PushPull, Low, ExtiIT, Rising, 0, 15},
     {PinId::CLI_TX, PortA, 2, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {PinId::CLI_RX, PortA, 3, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {PinId::LD_GRE, PortD, 12, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {PinId::LD_ORA, PortD, 13, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {PinId::LD_RED, PortD, 14, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {PinId::LD_BLU, PortD, 15, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15}}
};

/** (*Note 1) Alternate function mapping number for STM32: USART2 on PA2/PA3 is GPIO_AF7_USART2 = 7 */
/** (*Note 2) CLI = Command Line Interface, used for serial console over UART */

// Compile-Time std::array Optimization
constexpr const PinConfig& getPinConfigIndexed(PinId id)
{
    return gpioPinConfigs[static_cast<std::size_t>(id)];
}

#endif // GPIO_CONFIG_STM32_HPP
