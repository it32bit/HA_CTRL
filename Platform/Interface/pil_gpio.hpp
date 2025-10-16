/**
 * @file pil_gpio.hpp
 * @author it32bit
 * @brief GPIO interface definitions for platform-independent GPIO management.
 * @version 1.0
 * @date 2025-10-15
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2024 ha-ctrl project authors.
 */

#ifndef PIL_GPIO_HPP
#define PIL_GPIO_HPP

#include <cstdint>
#include <string_view>

class IGPIOPin
{
  public:
    virtual ~IGPIOPin() = default;

    virtual void toggle()     = 0;
    virtual void set()        = 0;
    virtual void reset()      = 0;
    virtual bool read() const = 0;

    virtual std::string_view name() const = 0;
};

class IGPIOManager
{
  public:
    virtual ~IGPIOManager() = default;

    // Get a pin controller by name. Return nullptr if not found.
    virtual IGPIOPin* getPin(std::string_view pinName) = 0;

    // std::optional<IGPIOPin*> getPin(std::string_view t_name);
};

#endif // PIL_GPIO_HPP
