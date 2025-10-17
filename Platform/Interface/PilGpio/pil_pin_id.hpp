/**
 * @file Platform/Interface/PilGpio/pin_id.hpp
 * @author it32bit
 * @brief Provides a compile-time mapping of named pins using the PinId enum class.
 *        Enables type-safe, efficient access to GPIO ports and pins without relying on runtime string parsing.
 *
 * @version 1.0
 * @date 2025-10-17
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef PIL_PIN_ID_HPP
#define PIL_PIN_ID_HPP

#include <stdint-gcc.h>

enum class PinId : uint8_t
{
    BUTTON,
    CLI_TX,
    CLI_RX,
    LD_GRE,
    LD_ORA,
    LD_RED,
    LD_BLU,
    // Always last
    COUNT
};

#endif // PIL_PIN_ID_HPP