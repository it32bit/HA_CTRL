/**
 * @file pin_config.hpp
 *
 * @author it32bit
 *
 * @version 0.1
 *
 * @brief This header defines a hardware-agnostic pin configuration structure.
 *
 * @note This can be used by both bootloader and application code
 *       to configure GPIO pins without including any hardware-specific headers.
 *       No HAL or hardware-specific headers should be included here.
 *       The actual implementation of pin control (set, reset, toggle, read) will be
 *       done in platform-specific code (e.g., STM32F4) using this configuration.
 *       The actual GPIO operations will be implemented in platform-specific files
 *       (e.g., gpio_pin_stm32.hpp/cpp) that use this configuration data
 *       to control the pins.
 *
 *  Example usage:
 *      constexpr PinConfig pinConfigs[] = {
 *          {"LED1", 0, 5, PinConfig::Mode::Output, PinConfig::Pull::None, 0},
 *          {"BUTTON1", 0, 13, PinConfig::Mode::Input, PinConfig::Pull::Up, 0},  // etc.
 *      };
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2024 ha-ctrl project authors.
 *            SPDX-License-Identifier: MIT
 */
#ifndef _PIN_CONFIG_HPP_
#define _PIN_CONFIG_HPP_

#include <cstdint>
#include <string_view>
#include <array>

struct PinConfig
{
    enum class Mode
    {
        Input,
        Output,
        Alternate,
        Analog
    };
    enum class Pull
    {
        None,
        Up,
        Down
    };

    std::string_view name;
    uint8_t          port;              // e.g., 0 for GPIOA, 1 for GPIOB, etc.
    uint16_t         pinNumber;         // Pin number within the port (0-15)
    Mode             mode;              // Input, Output, Alternate, Analog
    Pull             pull;              // None, Up, Down
    uint8_t          alternateFunction; // Alternate function number (0-15),
                                        // if applicable, 0 otherwise.
};

#endif // _PIN_CONFIG_HPP_
