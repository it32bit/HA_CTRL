/**
 * @file      Platform/STM32F4/Inc/adc_manager_stm32.hpp
 * @author    it32bit
 * @brief     Declares AdcManager for static instantiation and access to the ADC driver.
 *            Provides safe initialization and temperature reading without dynamic allocation.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#ifndef ADC_MANAGER_STM32_HPP
#define ADC_MANAGER_STM32_HPP

#include "pil_adc.hpp"
#include <cstddef>
#include <cstdint>

class AdcManager
{
  public:
    AdcManager();
    void  initialize();
    float readTemperature();

  private:
    static constexpr std::size_t MaxAdcSize  = sizeof(uint32_t) * 8;
    static constexpr std::size_t MaxAdcAlign = alignof(uint32_t);

    alignas(MaxAdcAlign) std::byte m_storage[MaxAdcSize];
    IAdc* m_adc = nullptr;
};

#endif
