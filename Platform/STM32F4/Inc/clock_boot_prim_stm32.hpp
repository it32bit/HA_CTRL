/**
 ******************************************************************************
 * @file        clock_boot_prim_stm32.hpp
 * @author      it32bit
 * @version     1.1
 * @date        2025-10-22
 * @brief       STM32F4 system clock configuration for Primary Bootloader
 *
 *              Declares the Clock_BootPrim class, which implements the IClockConfigurator
 *              interface and provides minimal system clock setup using the internal HSI oscillator.
 *              This configuration avoids PLL setup and uses default reset values for simplicity.
 *
 *              - System Clock Source            : HSI (internal 16 MHz)
 *              - PLL                            : Not used
 *              - SYSCLK                         : 16 MHz
 *              - AHB Prescaler (HCLK)           : 1 → 16 MHz
 *              - APB1 Prescaler                 : 1 → 16 MHz
 *              - APB2 Prescaler                 : 1 → 16 MHz
 *              - Flash Latency                  : Default (0 WS)
 *              - VDD                            : 3.3 V
 *
 *              The watchdog (IWDG) is disabled if possible. No external oscillator (HSE) or
 *              voltage scaling is configured. This setup is suitable for fast boot and minimal power.
 *
 *              An optional error handler can be passed to handle clock setup failures.
 *
 * @attention   This file is part of the ha-ctrl project and is licensed under the MIT License.
 *              (c) 2025 ha-ctrl project authors.
 ******************************************************************************
 */
#ifndef CLOCK_BOOT_PRIM_STM32_HPP
#define CLOCK_BOOT_PRIM_STM32_HPP

#include "pil_clock_config.hpp"

class Clock_BootPrim : public IClockConfigurator
{
  public:
    void configure(void (*handler)()) override;
};

#endif // CLOCK_BOOT_PRIM_STM32_HPP
