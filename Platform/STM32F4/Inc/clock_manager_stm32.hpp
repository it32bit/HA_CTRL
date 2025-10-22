/**
 ******************************************************************************
 * @file        clock_manager.hpp
 * @author      it32bit
 * @version     0.1
 * @date        2025-10-18
 * @brief       Clock management class interface.
 *
 *              Provides an abstraction to manage and initialize clock configuration
 *              in an embedded system. It uses an IClockConfigurator interface
 *              to apply platform-specific clock setup.
 *
 * @note        The clock configuration handler is passed during initialization.
 *              Internal storage is aligned and sized to hold clock-related data.
 *
 * @see         IClockConfigurator for clock configuration interface definition.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef CLOCK_MANAGER_HPP
#define CLOCK_MANAGER_HPP

#include <cstddef>
#include <cstdint>
#include "pil_clock_config.hpp"

class ClockManager
{
  public:
    ClockManager();

    /**
     * @brief Initialize the clock system.
     * @param handler A function pointer to a clock initialization handler.
     */
    void initialize(void (*handler)());

  private:
    static constexpr std::size_t MaxClockSize = sizeof(uint32_t) * 8;

    // Ensures proper memory alignment for safe placement of the object
    static constexpr std::size_t MaxClockAlign = alignof(uint32_t);

    IClockConfigurator* m_clock = nullptr;
    /**
     * @brief Aligned storage to hold clock data or objects
     * @note  This is a raw memory buffer where the actual clock object will be constructed.
     *        It’s statically allocated, avoiding dynamic memory (new/delete),
     *        'alignas' ensures the buffer meets alignment requirements for the object type.
     */
    alignas(MaxClockAlign) std::byte m_storage[MaxClockSize];
};

#endif // CLOCK_MANAGER_HPP
