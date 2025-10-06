#include <stdint-gcc.h>
#include <cstdio>
#include <math.h>
#include "hal_adc.hpp"
#include "stm32f4xx_ll_adc.h" // IWYU pragma: keep
#include "stm32f4xx_ll_bus.h"
#include "stm32f4xx_ll_utils.h"

// Pointers to the factory-calibrated ADC values stored in Flash memory
// These are read-only (const) and their addresses are fixed (const pointer).
const uint16_t* const TS_CAL1_ADDR =
    reinterpret_cast<const uint16_t*>(0x1FFF7A2C); // V_sense at 30°C
const uint16_t* const TS_CAL2_ADDR =
    reinterpret_cast<const uint16_t*>(0x1FFF7A2E); // V_sense at 110°C
const uint16_t* const VREFINT_ADDR = reinterpret_cast<const uint16_t*>(0x1FFF7A2A); // V_refint

uint16_t adc_rowData = 0;

uint16_t temp_raw = 0;
uint16_t vref_raw = 0;

static float convert_to_celsius(uint16_t adc_data);

static void ADC_Read_Internal();
static void ADC_Clock_Init();

/**
 * Formula from RM0090
 *      Temperature (°C) = ((T_raw * VREFINT_CAL / VREFINT_raw) - TS_CAL1)
 *                 * (110 - 30) / (TS_CAL2 - TS_CAL1) + 30
 */
static float convert_to_celsius(uint16_t adc_data)
{
    uint16_t ts_cal1     = *TS_CAL1_ADDR; // ADC at 30*C
    uint16_t ts_cal2     = *TS_CAL2_ADDR; // ADC at 110*C
    uint16_t vrefint_cal = *VREFINT_ADDR; // VREFINT at 3.3V

    // printf("temp_raw: %d\tvref_raw: %d\t c30: %d\tc110: %d\tvrefint: %d\t", temp_raw, vref_raw,
    //        ts_cal1, ts_cal2, vrefint_cal);

    // Normalize temp_raw to calibrated 3.3V reference
    float temp_corrected = ((float)temp_raw * vrefint_cal) / vref_raw;

    // Linear interpolation
    float temperature =
        ((temp_corrected - ts_cal1) * (110.0f - 30.0f)) / (ts_cal2 - ts_cal1) + 30.0f;

    return temperature;
}

float getTemperatureInCelcius()
{
    // Adc_Temperature_Start();
    // return convert_to_celsius(adc_rowData);

    ADC_Read_Internal();

    return convert_to_celsius(temp_raw);
}

void ADC_Internal_Init()
{
    ADC_Clock_Init();

    // Ensure ADC is disabled before configuration
    if (LL_ADC_IsEnabled(ADC1) == 1)
    {
        LL_ADC_Disable(ADC1);
        while (LL_ADC_IsEnabled(ADC1) == 1)
        {
        }; // Wait until disabled
    }

    // Configure ADC clock prescaler (PCLK2/4)
    LL_ADC_SetCommonClock(__LL_ADC_COMMON_INSTANCE(ADC1), LL_ADC_CLOCK_SYNC_PCLK_DIV4);

    // Set resolution to 12 bits
    LL_ADC_SetResolution(ADC1, LL_ADC_RESOLUTION_12B);

    // Set right data alignment
    LL_ADC_SetDataAlignment(ADC1, LL_ADC_DATA_ALIGN_RIGHT);

    // Enable scan mode for multiple channels
    LL_ADC_SetSequencersScanMode(ADC1, LL_ADC_SEQ_SCAN_ENABLE);

    // Set sampling time for channel 16 and 17 to 480 cycles (required for internal channels)
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_TEMPSENSOR, LL_ADC_SAMPLINGTIME_480CYCLES);
    LL_ADC_SetChannelSamplingTime(ADC1, LL_ADC_CHANNEL_VREFINT, LL_ADC_SAMPLINGTIME_480CYCLES);

    // Configure sequence: 2 conversions
    LL_ADC_REG_SetSequencerLength(ADC1, LL_ADC_REG_SEQ_SCAN_ENABLE_2RANKS);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_TEMPSENSOR);
    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_2, LL_ADC_CHANNEL_VREFINT);

    // Enable EOCS behavior LL_ADC_REG_FLAG_EOC_UNITARY_CONV: EOC flag is after each conversion
    LL_ADC_REG_SetFlagEndOfConversion(ADC1, LL_ADC_REG_FLAG_EOC_UNITARY_CONV);

    // Set continuous mode if needed, or single conversion
    LL_ADC_REG_SetContinuousMode(ADC1, LL_ADC_REG_CONV_SINGLE);

    // Set trigger to software
    LL_ADC_REG_SetTriggerSource(ADC1, LL_ADC_REG_TRIG_SOFTWARE);

    // Enable ADC
    LL_ADC_Enable(ADC1);

    // Wait for ADC ready
    while (!LL_ADC_IsEnabled(ADC1))
    {
    };

    // Wait after enabling temperature sensor (minimum 10 us)
    LL_mDelay(1);
}

static void ADC_Clock_Init(void)
{
    // Enable ADC1 clock
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_ADC1);

    // Enable SYSCFG (not strictly needed here but good for general internal setup)
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);

    // Enable temperature sensor and VREFINT
    LL_ADC_SetCommonPathInternalCh(__LL_ADC_COMMON_INSTANCE(ADC1),
                                   LL_ADC_PATH_INTERNAL_TEMPSENSOR | LL_ADC_PATH_INTERNAL_VREFINT);
}

static void ADC_Read_Internal()
{
    // Start conversion
    LL_ADC_REG_StartConversionSWStart(ADC1);

    // Wait for EOC for rank 1 (temperature sensor)
    while (!LL_ADC_IsActiveFlag_EOCS(ADC1))
    {
    };
    temp_raw = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOCS(ADC1); // Clear flag!

    // Wait for EOC again for rank 2 (VREFINT)
    while (!LL_ADC_IsActiveFlag_EOCS(ADC1))
    {
    };
    vref_raw = LL_ADC_REG_ReadConversionData12(ADC1);
    LL_ADC_ClearFlag_EOCS(ADC1); // Clear flag!
}
