#include "../inc/adc.h"
#include "stm32g0xx_hal.h"

void adc_start_conversion(ADC_Handle *adc) {
    HAL_ADC_Start((ADC_HandleTypeDef *)adc);
}

uint16_t adc_get_memory_result(ADC_Handle *adc, uint32_t channel) {
    ADC_HandleTypeDef *hadc = (ADC_HandleTypeDef *)adc;
    HAL_ADC_PollForConversion(hadc, 1); // non-blocking or short timeout
    return HAL_ADC_GetValue(hadc);
}

void adc_enable_conversions(ADC_Handle *adc) {
    // STM HAL ADC is usually always enabled after init.
    // Optional: Reconfigure if needed.
}

float adc_get_value(ADC_Handle *adc, uint32_t channel) {
    uint16_t raw = adc_get_memory_result(adc, channel);
    return (float)raw / 4095.0f;
}

float adc_get_voltage(ADC_Handle *adc, uint32_t channel, float vref) {
    return adc_get_value(adc, channel) * vref;
}
