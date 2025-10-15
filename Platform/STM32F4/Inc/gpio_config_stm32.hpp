/**
 ******************************************************************************
 * @file        : gpio_config_stm32.hpp
 * @author      : i32bit
 * @brief       : GPIO configuration and label mapping header
 *                for board-level initialization.
 ******************************************************************************
 * This header provides compile-time arrays for GPIO pin setup and name-to-index mapping.
 * The configuration array `gpioPinConfigs` defines hardware parameters and labels for each pin,
 * enabling platform-agnostic initialization code to configure GPIOs based on this data.
 *
 * Usage for Application and Bootloader:
 * @code
 *  #include "gpio_manager_stm32.hpp"
 *
 *  GpioManager gpio;
 *  gpio.initialize(gpioPinConfigs);
 *  auto led = gpio.getPin("LD_RED");
 *  
 *  led->set();
 * @endcode
 */
#ifndef GPIO_CONFIG_STM32_HPP
#define GPIO_CONFIG_STM32_HPP

#include <array>
#include <string_view>
#include "pil_pin_config.hpp"
#include "gpio_pin_stm32.hpp"

using enum PinConfig::Mode;
using enum PinConfig::Pull;
using enum PinConfig::Type;
using enum PinConfig::Speed;
using enum PinConfig::InterruptTrigger;
using enum PinConfig::PortStm32;
using enum PinConfig::InterruptExti;

constexpr std::size_t PIN_CONFIG_ARRAY_SIZE = 7;

/**
 * @brief   GPIO PIN configuration array
 *
 * @details This will be located in flash, and the configure function will consume the data in pace,
 *          without the need to populate structs in stack memory
 */
constexpr std::array<PinConfig, PIN_CONFIG_ARRAY_SIZE> gpioPinConfigs = {
    {{"BUTTON", PortA, 0, Input, PullDown, PushPull, Low, ExtiIT, Rising, 0, 15},
     {"CLI_TX", PortA, 2, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {"CLI_RX", PortA, 3, Alternate, PullNone, PushPull, VeryHigh, ExtiNone, None, 7, 15},
     {"LD_GRE", PortD, 12, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD_ORA", PortD, 13, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD_RED", PortD, 14, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15},
     {"LD_BLU", PortD, 15, Output, PullNone, PushPull, Low, ExtiNone, None, 0, 15}}
};

/** (*Note 1) Alternate function mapping number for STM32: USART2 on PA2/PA3 is GPIO_AF7_USART2 = 7 */
/** (*Note 2) CLI = Command Line Interface, used for serial console over UART */

// Static assert to ensure the array size matches the defined size
// static_assert(sizeof(gpioPinConfigs) / sizeof(gpioPinConfigs[0]) == PIN_CONFIG_ARRAY_SIZE, "GPIO config array size mismatch");

/**
 * @brief Find a PinConfig by its name.
 * This function performs a linear search through the gpioPinConfigs array
 * to find a PinConfig with the specified name.
 * @param t_name The name of the pin to find.
 * @return A pointer to the PinConfig if found, or nullptr if not found.
 * @note This function is constexpr and can be used in compile-time contexts.
 *
 * Usage:
 * constexpr auto* buttonCfg = findPinConfig("BUTTON");
 * GpioPin_STM32* buttonPin = GpioPin_STM32::createStatic(*buttonCfg);
 * buttonPin->set();
 */
// Compile-time lookup by name
constexpr const PinConfig* findPinConfig(std::string_view t_name)
{
    for (const auto& cfg : gpioPinConfigs)
    {
        if (cfg.name == t_name)
        {
            return &cfg;
        }
    }
    return nullptr;
}

#endif // GPIO_CONFIG_STM32_HPP
