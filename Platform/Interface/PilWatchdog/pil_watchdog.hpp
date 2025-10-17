/**
 ******************************************************************************
 * @file        pil_watchdog.hpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-17
 * @brief       Abstract watchdog interface for platform-independent implementation.
 *
 *              Defines the IWatchdog interface, which provides a virtual method
 *              for feeding a watchdog timer. Enables mocking for unit tests and
 *              allows platform-specific watchdog implementations to be swapped
 *              seamlessly in embedded systems.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef PIL_WATCHDOG_HPP
#define PIL_WATCHDOG_HPP

class IWatchdog
{
  public:
    virtual void feed()  = 0;
    virtual ~IWatchdog() = default;
};

#endif // I_WATCHDOG_HPP
