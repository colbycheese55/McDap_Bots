/*
 * Copyright (c) 2023, Texas Instruments Incorporated
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include "ti_msp_dl_config.h"
#include "LowLevelDrivers/inc/gpio.h"
#include "HighLevelDrivers/inc/SSD1306.h"
// #include "LowLevelDrivers/inc/adc.h"

/* This results in approximately 0.5s of delay assuming 32MHz CPU_CLK */
#define DELAY (16000000)

volatile bool gCheckADC;
volatile uint16_t gAdcResult;

int main(void)
{
    /* Power on GPIO, initialize pins as digital outputs */
    // SYSCFG_DL_init();
    
    /* Initialize ADC and enable NVIC interrupt */
    // adc_init(ADC12_0_INST);

    /* Default: LED1 and LED3 ON, LED2 OFF */
    DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_2_PIN | GPIO_LEDS_USER_LED_3_PIN | GPIO_LEDS_USER_LED_1_PIN);
    // DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN |
                                        // GPIO_LEDS_USER_LED_3_PIN |
    //                                     // GPIO_LEDS_USER_TEST_PIN);
    // float val = 0;
    // while (1) {
    //     /*
    //      * Call togglePins API to flip the current value of LEDs 1-3. This
    //      * API causes the corresponding HW bits to be flipped by the GPIO HW
    //      * without need for additional R-M-W cycles by the processor.
    //      */
    //     delay_cycles(DELAY);

    //     val = adc_get_voltage(ADC12_0_INST);

    //     // DL_GPIO_togglePins(GPIO_LEDS_PORT,
    //         // GPIO_LEDS_USER_LED_1_PIN | GPIO_LEDS_USER_LED_2_PIN |
    //             // GPIO_LEDS_USER_LED_3_PIN | GPIO_LEDS_USER_TEST_PIN);

    //     // val = gpio_read_pin(GPIO_LEDS_PORT, GPIO_LEDS_PIN_0_PIN);
    //     if (val > 1) {
    //         gpio_write_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN, 0);
    //     } else {
    //         gpio_write_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN, 0);
    //     }

    //     // gpio_write_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_2_PIN, 1);

    //     // val = gpio_read_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
    //     // val = ~val;
    //     // gpio_write_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN, val);
        
    // }


    SYSCFG_DL_init();
    
    // Try configuring ADC to read internal temperature sensor instead of external pin
    // This will test if ADC hardware is working
    // DL_ADC12_configConversionMem(ADC12_0_INST, DL_ADC12_MEM_IDX_0,
    //     DL_ADC12_REFERENCE_VOLTAGE_INTREF, 
    //     DL_ADC12_SAMPLE_TIMER_SOURCE_SCOMP0, DL_ADC12_AVERAGING_MODE_DISABLED,
    //     DL_ADC12_BURN_OUT_SOURCE_DISABLED, DL_ADC12_TRIGGER_MODE_AUTO_NEXT, 
    //     DL_ADC12_WINDOWS_COMP_MODE_DISABLED);
        
    DL_ADC12_enableConversions(ADC12_0_INST);
    
    // Manually configure PA27 as ADC input (Channel 0) to override sysconfig
    // DL_GPIO_initPeripheralAnalogFunction(IOMUX_PINCM59);

    NVIC_EnableIRQ(ADC12_0_INST_INT_IRQN);
    gCheckADC = true;

    // 1. Initialize the OLED using internal voltage source
    SSD1306_Init(SSD1306_SWITCHCAPVCC);

    // 2. Clear display memory
    SSD1306_OutClear();

    // 3. Move cursor to top-left (X=0, Y=0)
    SSD1306_SetCursor(0, 0);

    // 4. Output a string
    SSD1306_OutString("Hello, world!");

    while (1) {
        DL_ADC12_startConversion(ADC12_0_INST);

        while (false == gCheckADC) {
            __WFE();
        }

        gAdcResult = DL_ADC12_getMemResult(ADC12_0_INST, DL_ADC12_MEM_IDX_0);

        // Debug: Try reading other memory locations and raw registers
        uint16_t raw_memres = ADC12_0_INST->ULLMEM.MEMRES[0];
        uint16_t status_reg = ADC12_0_INST->ULLMEM.STATUS;
        
        // Test with a known value - try reading internal temperature or voltage reference
        // This will help us determine if the ADC hardware is working at all

        if (gAdcResult > 0x0) {
            DL_GPIO_clearPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        } else {
            DL_GPIO_setPins(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_1_PIN);
        }
        gpio_toggle_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_2_PIN);
        // gCheckADC = false;
        DL_ADC12_enableConversions(ADC12_0_INST);
        // gpio_toggle_pin(GPIO_LEDS_PORT, GPIO_LEDS_USER_LED_3_PIN);
        delay_cycles(DELAY);
    }
}


// void ADC12_0_INST_IRQHandler(void)
// {
    
//     switch (DL_ADC12_getPendingInterrupt(ADC12_0_INST)) {
//         case DL_ADC12_IIDX_MEM0_RESULT_LOADED:
            
//             gCheckADC = true;
//             break;
//         default:
//             break;
//     }
// }