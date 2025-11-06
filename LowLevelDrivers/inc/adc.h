#ifndef ADC_H_
#define ADC_H_

#include "ti_msp_dl_config.h"


// begin converting an analog input to digital
void adc_start_conversion(ADC12_Regs *adc12);

// read the ADC memory to get the digital value. Does NOT wait for conversion to complete
uint16_t adc_get_memory_result(const ADC12_Regs *adc12, DL_ADC12_MEM_IDX idx);

// reenable the ADC after completing the conversion
void adc_enable_conversions(ADC12_Regs *adc12);


// read the ADC for the current analog value, returns a float between 0.0 and 1.0
float adc_get_value(ADC12_Regs *adc12);

// read the ADC for the current voltage, returns a float between 0.0 and 3.3
float adc_get_voltage(ADC12_Regs *adc12);

#endif /* ADC_H_ */