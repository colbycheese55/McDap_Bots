#include "../inc/adc.h"

volatile bool adcDone = false; // ADC conversion completion flag

void adc_start_conversion(ADC_Handle *adc) {
	ADC12_Regs *adc12 = adc->adc12;
    adc12->ULLMEM.CTL1 |= (ADC12_CTL1_SC_START);
}

uint16_t adc_get_memory_result(ADC_Handle *adc) {
	ADC12_Regs *adc12 = adc->adc12;
	DL_ADC12_MEM_IDX idx =  adc->channel;

    volatile const uint32_t *pReg = &adc12->ULLMEM.MEMRES[idx];

    return (uint16_t)(*(pReg + DL_ADC12_SVT_OFFSET));
}

void adc_enable_conversions(ADC_Handle *adc) {
	ADC12_Regs *adc12 = adc->adc12;
    adc12->ULLMEM.CTL0 |= (ADC12_CTL0_ENC_ON);
}

float adc_get_value(ADC_Handle *adc) {
    adc_start_conversion(adc);
    while (!adcDone) {
        __WFE();
    }
    adcDone = false;
    uint16_t raw = adc_get_memory_result(adc);
    adc_enable_conversions(adc);

    return raw / 4095.0f;
}

float adc_get_voltage(ADC_Handle *adc, float vref) {
    return adc_get_value(adc) * 3.3f;
}


// --------- ADC INTERRUPT HANDLER ---------
void ADC12_0_INST_IRQHandler(void)
{
    // switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
    //     case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
    //         adcDone = true;
    //         break;
    //     default:
    //         break;
    // }
}
