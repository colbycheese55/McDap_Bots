// main_pwm.c
#include <stdint.h>
#include "ti_msp_dl_config.h"   // gives PWM_0_INST, DL_TimerG_*, __NOP

/* crude delay; just for demo pacing */
static void delay_ms(uint32_t ms)
{
    volatile uint32_t cycles = ms * 1000u; // ~32 MHz -> ~1ms per 32000 loops
    while (cycles--) { __NOP(); }
}

int main(void)
{
    SYSCFG_DL_init();  // pin mux: CC0->PA1 (unobservable), CC1->PA0 (LED)

    /* Friendlier LED frequency: 1 kHz → LOAD=32000 at 32 MHz */
    const uint32_t load = 32000u;
    DL_TimerG_setLoadValue(PWM_0_INST, load);

    /* ===== Case 1: BOTH ON =====
       CC0 @ 60% (PA1, not visible), CC1 @ 30% (PA0 LED will “dim”) */
    pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.60f);  // PA1 (not visible)
    pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.30f);  // PA0 LED visible
    DL_TimerG_startCounter(PWM_0_INST);
    delay_ms(1500);

    /* ===== Case 2: ONLY CC0 ON =====
       Park CC1 by setting duty to 0% → PA0 LED steady/off.
       CC0 keeps PWM (still not visible without a probe on PA1). */
    pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.0f);   // PA0 steady
    pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.70f);  // PA1 running (unseen)
    delay_ms(1500);

    /* ===== Case 3: ONLY CC1 ON =====
       Park CC0 (0%), run CC1 (LED) at 80% → PA0 LED bright PWM. */
    pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.0f);   // PA1 parked
    pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.80f);  // PA0 LED PWM
    delay_ms(1500);

    /* Loop: repeat the three cases so you can observe on PA0 */

    int count = 0;
    while (1) {
        // // both on
        // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.50f);
        // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.10f);  // PA0 visible
        // delay_ms(1000);

        // // only CC0 (PA0 steady)
        // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.0f);
        // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.70f);
        // delay_ms(1000);

        // // only CC1 (PA0 PWM)
        // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.0f);
        // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.50f);
        // delay_ms(1000);

        pwm_set_duty(DL_TIMER_CC_0_INDEX, load, (count % 100) / 100.0);
        pwm_set_duty(DL_TIMER_CC_1_INDEX, load, (count % 100) / 100.0);
        count += 1;
        delay_ms(50);
    }
}











