#include <stdbool.h>
#include <stdint.h>
#include "../inc/adc.h"

// forward declarations
void adc_start_conversion(ADC12_Regs *instance);
uint16_t adc_get_mem(ADC12_Regs *instance, DL_ADC12_MEM_IDX mem_idx);
void adc_enable_conversion(ADC12_Regs *instance);


bool waiting = false;


void adc_init(ADC12_Regs *instance_init)
{
    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
}

float adc_get_value(ADC12_Regs *instance) {
    adc_start_conversion(instance);

    while (waiting);

    uint16_t raw = adc_get_mem(instance, ADC12_0_ADCMEM_0);
    float ret = raw / 4095.0f;
    adc_enable_conversion(instance);
    return ret;
}

float adc_get_voltage(ADC12_Regs *instance) {
    return adc_get_value(instance) * 3.3f;
}

void adc_start_conversion(ADC12_Regs *instance) {
        instance->ULLMEM.CTL1 |= (ADC12_CTL1_SC_START);
}

uint16_t adc_get_mem(ADC12_Regs *instance, DL_ADC12_MEM_IDX mem_idx) {
    // Now flexible - can access any memory slot
    volatile const uint32_t *pReg = &instance->ULLMEM.MEMRES[mem_idx];
    return (uint16_t)(*(pReg + DL_ADC12_SVT_OFFSET));
}

void adc_enable_conversion(ADC12_Regs *instance) {
        instance->ULLMEM.CTL0 |= (ADC12_CTL0_ENC_ON);
}


// ************* Interrupt Handlers *************
void ADC12_0_INST_IRQHandler(void) {
    if (ADC12_0_INST->ULLMEM.CPU_INT.IIDX == DL_ADC12_IIDX_MEM0_RESULT_LOADED) {
        waiting = false;
    }
}