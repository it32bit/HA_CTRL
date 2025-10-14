/**
 * @file pil_pin_config.hpp
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
#include <stdint-gcc.h>

struct PinConfig
{
    enum Mode : uint32_t
    {
        Input     = 0,
        Output    = 1,
        Alternate = 2,
        Analog    = 3
    };

    enum Pull : uint32_t
    {
        PullNone = 0,
        PullUp   = 1,
        PullDown = 2
    };

    enum Type : uint32_t
    {
        PushPull  = 0,
        OpenDrain = 1
    };

    enum Speed : uint32_t
    {
        Low      = 0,
        Medium   = 1,
        High     = 2,
        VeryHigh = 3
    };

    enum InterruptTrigger : uint32_t
    {
        None          = 0,
        Rising        = 1,
        Falling       = 2,
        RisingFalling = 3
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

    enum InterruptExti : uint32_t
    {
        ExtiNone = 0,
        ExtiIT   = 1,
        ExtiEVT  = 2
    };

    std::string_view name;
    uint8_t          portIndex;
    uint16_t         pinNumber; // Pin number within the port (0-15) for SMT32 or (0-32) for ESP32
    Mode             mode;      // Input, Output, Alternate, Analog
    Pull             pull;      // None, Up, Down
    Type             type;
    Speed            speed;
    InterruptExti    iExti;       // Interrupt or Event line, if applicable
    InterruptTrigger iTrigger;    // Interrupt trigger type, if applicable
    uint32_t         altFunction; // Alternate function number (0-15), 0 = none.
    uint32_t         iPriority;   // NVIC priority (optional)
};

#endif // _PIN_CONFIG_HPP_
