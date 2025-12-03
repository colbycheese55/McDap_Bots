#ifndef HAL_ADC_H
#define HAL_ADC_H


#include <stdint.h>

#ifdef STM32G071xx
#include "stm32g0xx_hal.h"

typedef struct {
	ADC_HandleTypeDef* hadc;
	uint32_t channel;
} ADC_Handle;

#elif __MSPM0G3507__
#include "ti_msp_dl_config.h"

typedef struct {
	ADC12_Regs* adc12;
	DL_ADC12_MEM_IDX channel;
	uint32_t mem_result_loaded;
} ADC_Handle;

#endif

// read the ADC for the current analog value, returns a float between 0.0 and 1.0
float adc_get_value(ADC_Handle *adc);

// read the ADC for the current voltage, returns a float between 0.0 and 3.3
float adc_get_voltage(ADC_Handle *adc, float vref);

#endif //HAL_ADC_H
