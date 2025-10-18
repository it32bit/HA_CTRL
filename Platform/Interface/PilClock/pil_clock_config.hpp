/**
 * @file Platform/Interface/PilClock/pil_clock_config.hpp
 * @author it32bit
 * @brief Declares the IClockConfigurator interface for platform-specific system clock configuration.
 *        Enables flexible and testable clock setup with optional callback integration.
 *
 * @version 1.0
 * @date 2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef PIL_CLOCK_CONFIGURATOR_HPP
#define PIL_CLOCK_CONFIGURATOR_HPP

class IClockConfigurator
{
  public:
    virtual ~IClockConfigurator()                       = default;
    virtual void configure(void (*handler)() = nullptr) = 0;
};

#endif // PIL_CLOCK_CONFIGURATOR_HPP
