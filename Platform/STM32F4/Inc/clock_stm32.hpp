/**
 ******************************************************************************
 * @file        Clock_STM32.hpp
 * @author      it32bit
 * @version     1.0
 * @date        2025-10-18
 * @brief       STM32F4 system clock configuration interface.
 *
 *              Declares the Clock_STM32F4 class, which implements the IClockConfigurator
 *              interface and provides system clock setup using PLL with HSE as the source.
 *              The configuration targets the following parameters:
 *
 *              - System Clock source            : PLL (HSE)
 *              - SYSCLK                         : 168 MHz
 *              - HCLK                           : 168 MHz
 *              - AHB Prescaler                  : 1
 *              - APB1 Prescaler                 : 4 (42 MHz)
 *              - APB2 Prescaler                 : 2 (84 MHz)
 *              - HSE Frequency                  : 8 MHz
 *              - PLL_M                          : 8
 *              - PLL_N                          : 336
 *              - PLL_P                          : 2
 *              - PLL_Q                          : 7
 *              - VDD                            : 3.3 V
 *              - Regulator Output               : Scale1 mode
 *              - Flash Latency                  : 5 WS
 *
 *              An optional error handler can be passed to handle clock setup failures.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef CLOCK_STM32_HPP
#define CLOCK_STM32_HPP

#include "pil_clock_config.hpp"

class Clock_STM32F4 : public IClockConfigurator
{
  public:
    void configure(void (*handler)()) override;

  private:
    void configureSystemClock();
    void (*m_errorHandler)() = nullptr; // Optional error handler
};

#endif // CLOCK_STM32_HPP
