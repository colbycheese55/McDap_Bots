#include "pwm.h"

#define TIMER_CLOCK_HZ 32000000UL

/* Simple delay using CMSIS __NOP (available via ti_msp_dl_config.h pulled in by pwm.h) */
static void delay_cycles_block(uint32_t cycles) {
    while (cycles--) { __NOP(); }
}

int main(void)
{
    PWM_boardInit();  // wraps SYSCFG_DL_init(); sets PA0->TIMG8_CCP1, etc.

    PWM_Handle pwm;
    // Channel 1 = CCP1 -> PA0 (red LED on J4)
    PWM_init(&pwm, 1, TIMER_CLOCK_HZ, 1000, 0.5f);
    PWM_start(&pwm);

    while (1) {
        // Fade up
        for (float d = 0.0f; d <= 1.0f; d += 0.05f) {
            PWM_setDuty(&pwm, d);
            delay_cycles_block(32 * 100000);  // ~100 ms @ 32 MHz
        }
        // Fade down
        for (float d = 1.0f; d >= 0.0f; d -= 0.05f) {
            PWM_setDuty(&pwm, d);
            delay_cycles_block(32 * 100000);
        }
    }
}











