/**
 * @file      Platform/STM32F4/Src/adc_manager_stm32.cpp
 * @author    it32bit
 * @brief     Implements AdcManager for STM32F4 platform.
 *            Constructs Adc_STM32 in-place and delegates temperature reading via IAdc interface.
 *
 * @version   1.0
 * @date      2025-10-19
 * @attention This file is part of the ha-ctrl project and is licensed under the MIT License.
 *            (c) 2025 ha-ctrl project authors.
 */
#include "adc_manager_stm32.hpp"
#include "adc_stm32.hpp"
#include <new>

AdcManager::AdcManager() = default;

void AdcManager::initialize()
{
    m_adc = new (m_storage) Adc_STM32();
    m_adc->init();
}

float AdcManager::readTemperature()
{
    return m_adc ? m_adc->readTemperature() : 0.0f;
}
