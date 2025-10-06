#ifndef _HAL_ADC_HPP_
#define _HAL_ADC_HPP_

extern void Adc_Temperature_Init();
extern void Adc_Temperature_Start();

extern void ADC_Internal_Init();

extern float getTemperatureInCelcius();

#endif // _HAL_ADC_HPP_
