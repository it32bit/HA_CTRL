/**
 ******************************************************************************
 * @file        watchdog_stm32.hpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-17
 * @brief       Interface for STM32 Independent Watchdog (IWDG) driver.
 *
 *              Declares the Watchdog_STM32 class, which implements the IWatchdog
 *              interface and provides methods to configure and feed the IWDG peripheral.
 *              The watchdog timeout is calculated based on the LSI frequency and prescaler.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef WATCHDOG_STM32_HPP
#define WATCHDOG_STM32_HPP

#include "pil_watchdog.hpp"
#include <stdint.h>

class Watchdog_STM32 : public IWatchdog
{
  public:
    explicit Watchdog_STM32(uint32_t t_timeout_ms);
    void feed() override;

  private:
    void     waitReady();
    uint32_t m_reload_value;
};

#endif // WATCHDOG_STM32_HPP
