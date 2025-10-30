#ifndef HAL_ADC_H
#define HAL_ADC_H


#include <stdint.h>

typedef struct ADC_Handle ADC_Handle;


// begin converting an analog input to digital
void adc_start_conversion(ADC_Handle *adc);

// read the ADC memory to get the digital value. Does NOT wait for conversion to complete
uint16_t adc_get_memory_result(ADC_Handle *adc, uint32_t channel);

// reenable the ADC after completing the conversion
void adc_enable_conversions(ADC_Handle *adc);


// read the ADC for the current analog value, returns a float between 0.0 and 1.0
float adc_get_value(ADC_Handle *adc, uint32_t channel);

// read the ADC for the current voltage, returns a float between 0.0 and 3.3
float adc_get_voltage(ADC_Handle *adc, uint32_t channel, float vref);

#endif //HAL_ADC_H
