/**
 * @file      Platform/STM32F4/Inc/adc_stm32.hpp
 * @author    it32bit
 * @brief     Provides STM32F4-specific implementation of the IAdc interface.
 *            Wraps internal ADC configuration and temperature sensor reading using LL drivers.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */

#ifndef ADC_STM32_HPP
#define ADC_STM32_HPP

#include "pil_adc.hpp"

class Adc_STM32 : public IAdc
{
  public:
    void  init() override;
    float readTemperature() override;
};

#endif
