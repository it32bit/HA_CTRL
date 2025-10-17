/**
 * @file pil_gpio.hpp
 * @author it32bit
 * @brief GPIO interface definitions for platform-independent GPIO management.
 * @version 1.0
 * @date 2025-10-15
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */

#ifndef PIL_GPIO_HPP
#define PIL_GPIO_HPP

#include <cstdint>
#include <string_view>
#include "pil_pin_id.hpp"

class IGPIOPin
{
  public:
    virtual ~IGPIOPin() = default;

    virtual void toggle()     = 0;
    virtual void set()        = 0;
    virtual void reset()      = 0;
    virtual bool read() const = 0;
};

class IGPIOManager
{
  public:
    virtual ~IGPIOManager() = default;

    // Get a pin controller by name. Return nullptr if not found.
    virtual IGPIOPin* getPin(PinId t_id) = 0;
};

#endif // PIL_GPIO_HPP
