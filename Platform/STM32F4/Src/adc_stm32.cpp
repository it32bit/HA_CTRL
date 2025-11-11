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
#include <cstdint>
#include <cmath>
#include "adc_stm32.hpp"
#include "stm32f4xx_ll_adc.h"
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_utils.h"

namespace
{
// Factory-calibrated ADC values stored in Flash
constexpr uint32_t TS_CAL1_ADDR = 0x1FFF7A2C; // @30°C
constexpr uint32_t TS_CAL2_ADDR = 0x1FFF7A2E; // @110°C
constexpr uint32_t VREFINT_ADDR = 0x1FFF7A2A; // Vref

constexpr uint32_t ADC_TIMEOUT_MS = 10;

uint16_t temp_raw = 0;
uint16_t vref_raw = 0;

void enableAdcClock()
{
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                   LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT);
}

void configureAdc()
{
    if (LL_ADC_IsEnabled(ADC1))
    {
        LL_ADC_Disable(ADC1);
        while (LL_ADC_IsEnabled(ADC1))
        {
        }
    }

    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);
    LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);

    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_480CYCLES);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_480CYCLES);

    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_VREFINT);

    LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    LL_ADC_Enable(ADC1);
    while (!LL_ADC_IsEnabled(ADC1))
    {
    }

    LL_mDelay(1); // Wait for internal sensor stabilization
}

float convertToCelsius(uint16_t raw)
{
    const uint16_t ts_cal1     = *reinterpret_cast<const uint16_t*>(TS_CAL1_ADDR);
    const uint16_t ts_cal2     = *reinterpret_cast<const uint16_t*>(TS_CAL2_ADDR);
    const uint16_t vrefint_cal = *reinterpret_cast<const uint16_t*>(VREFINT_ADDR);

    const float corrected = static_cast<float>(temp_raw) * vrefint_cal / vref_raw;
    const float slope     = (110.0f - 30.0f) / (ts_cal2 - ts_cal1);

    return (corrected - ts_cal1) * slope + 30.0f;
}

void readInternalChannels()
{
    LL_ADC_REG_StartConversionSWStart(ADC1);

    while (!LL_ADC_IsActiveFlag_EOCS(ADC1))
    {
    }
    temp_raw = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOCS(ADC1);

    while (!LL_ADC_IsActiveFlag_EOCS(ADC1))
    {
    }
    vref_raw = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOCS(ADC1);
}

} // namespace

void Adc_STM32::init()
{
    enableAdcClock();
    configureAdc();
}

float Adc_STM32::readTemperature()
{
    readInternalChannels();
    return convertToCelsius(temp_raw);
}
