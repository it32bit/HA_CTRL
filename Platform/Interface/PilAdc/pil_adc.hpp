/**
 * @file      Platform/Interface/PilAdc/pil_adc.hpp
 * @author    it32bit
 * @brief     Declares the IAdc interface for platform-independent ADC access.
 *            Enables portable temperature sensing and analog input abstraction across MCU targets.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef PIL_ADC_HPP
#define PIL_ADC_HPP

class IAdc
{
  public:
    virtual void  init()            = 0;
    virtual float readTemperature() = 0;
    virtual ~IAdc()                 = default;
};

#endif
