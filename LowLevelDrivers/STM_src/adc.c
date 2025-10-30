#include "../inc/adc.h"

void init_adc(ADC_Handle* adc, size_t adc_ptr, uint32_t channel){
	adc->hadc = (ADC_HandleTypeDef *) adc_ptr;
}

void adc_start_conversion(ADC_Handle *adc) {
    HAL_ADC_Start(adc->hadc);
}

uint16_t adc_get_memory_result(ADC_Handle *adc) {
    ADC_HandleTypeDef *hadc = adc->hadc;
    HAL_ADC_PollForConversion(hadc, 1); // non-blocking or short timeout
    return HAL_ADC_GetValue(hadc);
}

void adc_enable_conversions(ADC_Handle *adc) {
    // STM HAL ADC is usually always enabled after init.
    // Optional: Reconfigure if needed.
}

float adc_get_value(ADC_Handle *adc) {
    uint16_t raw = adc_get_memory_result(adc);
    return (float)raw / 4095.0f;
}

float adc_get_voltage(ADC_Handle *adc, float vref) {
    return adc_get_value(adc) * vref;
}
