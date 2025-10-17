/**
 * @file pil_pin_config.hpp
 * @author it32bit
 * @version 0.1
 * @brief Defines a hardware-agnostic structure for GPIO pin configuration.
 *
 * @note This header can be used by both bootloader and application code to configure GPIO pins
 *       without including any hardware-specific dependencies. No HAL or platform-specific headers
 *       should be included here.
 *
 *       The actual implementation of pin control operations (e.g., set, reset, toggle, read)
 *       is delegated to platform-specific code (such as STM32F4), which uses this configuration
 *       structure to manage GPIO behavior.
 *
 *       GPIO operations are implemented in platform-specific files (e.g., gpio_pin_stm32.hpp/cpp)
 *       that interpret this configuration data to control the hardware.
 *
 * @example
 *       constexpr PinConfig pinConfigs[] = {
 *           {PinId::LD_RED, 0, 5, PinConfig::Mode::Output, PinConfig::Pull::PullNone, 0},
 *           {PinId::BUTTON, 0, 13, PinConfig::Mode::Input, PinConfig::Pull::PullUp, 0},
 *       };
 *
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef _PIN_CONFIG_HPP_
#define _PIN_CONFIG_HPP_

#include <cstdint>
#include <stdint-gcc.h>
#include "pil_pin_id.hpp"

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

    enum PortStm32 : uint8_t
    {
        PortA = 0, // e.g., for STM32 and similar MCUs
        PortB = 1,
        PortC = 2,
        PortD = 3,
        PortE = 4,
        PortF = 5,
        PortG = 6,
        PortH = 7,
        PortI = 8
    };

    enum PortEsp32 : uint8_t
    {
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

    PinId            id;
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

// Compile-time lookup function.
template <std::size_t N>
constexpr const PinConfig& getPinConfigById(PinId t_id, const PinConfig (&t_configs)[N])
{
    for (std::size_t i = 0; i < N; ++i)
    {
        if (t_configs[i].id == t_id)
        {
            return t_configs[i];
        }
    }
    // Force a compile-time error if not found
    static_assert(N < 0, "PinId not found in gpioPinConfigs array");

    // fallback to suppress warning
    return t_configs[0];
}

#endif // _PIN_CONFIG_HPP_
