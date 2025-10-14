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
        PullNone,
        PullUp,
        PullDown
    };
    enum class Type
    {
        PushPull,
        OpenDrain
    };
    enum class Speed
    {
        Low,
        Medium,
        High,
        VeryHigh
    };
    enum class InterruptTrigger
    {
        None,
        Rising,
        Falling,
        RisingFalling
    };

    enum PortIndex : uint8_t
    {
        PortA = 0, // e.g., for STM32 and similar MCUs
        PortB = 1,
        PortC = 2,
        PortD = 3,
        PortE = 4,
        PortF = 5,
        PortG = 6,
        PortH = 7,
        PortI = 8,
        Port0 = 0, // for ESP32 and similar MCUs
        Port1 = 1,
        Port2 = 2,
        Port3 = 3,
        Port4 = 4
    };

    std::string_view name;
    uint8_t          portIndex;
    uint16_t         pinNumber; // Pin number within the port (0-15) for SMT32 or (0-32) for ESP32
    Mode             mode;      // Input, Output, Alternate, Analog
    Pull             pull;      // None, Up, Down
    Type             type;
    Speed            speed;
    InterruptTrigger interrupt;         // Interrupt trigger type, if applicable
    uint8_t          alternateFunction; // Alternate function number (0-15), 0 = none.
    uint8_t          priority;          // NVIC priority (optional)
};

#endif // _PIN_CONFIG_HPP_
