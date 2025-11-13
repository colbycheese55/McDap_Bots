#include "../inc/adc.h"

float adc_get_value(ADC_Handle *adc) {
    HAL_ADC_Start(adc->hadc);
    HAL_ADC_PollForConversion(adc->hadc, 5); //adjust timeout value if needed
    uint16_t raw = HAL_ADC_GetValue(adc->hadc);
    return (float)raw / 4095.0f;
}

float adc_get_voltage(ADC_Handle *adc, float vref) {
    return adc_get_value(adc) * vref;
}
