#include "../inc/pwm.h"

/* --- Small helpers --- */
static inline void set_cc(PWM_Handle *h, uint32_t cmpTicks) {
    if (h->channel == 0) {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, cmpTicks, DL_TIMER_CC_0_INDEX);
    } else {
        DL_TimerG_setCaptureCompareValue(PWM_0_INST, cmpTicks, DL_TIMER_CC_1_INDEX);
    }
}

/* --- Public API --- */

void PWM_boardInit(void) {
    /* Pinmux, power, timer mode, CCP output config, etc. */
    SYSCFG_DL_init();
}

void PWM_init(PWM_Handle *h, uint8_t channel,
              uint32_t clkFreq, uint32_t pwmFreq, float duty)
{
    h->channel     = channel;
    h->clkFreq     = clkFreq;
    h->periodTicks = (pwmFreq == 0) ? 1 : (clkFreq / pwmFreq);

    /* Period (LOAD). For 1 kHz at 32 MHz → 32000 */
    DL_TimerG_setLoadValue(PWM_0_INST, h->periodTicks);

    /* Initial duty */
    PWM_setDuty(h, duty);
}

void PWM_setDuty(PWM_Handle *h, float duty)
{
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;

    uint32_t cmp = (uint32_t)(h->periodTicks * duty);
    set_cc(h, cmp);
}

void PWM_setFrequency(PWM_Handle *h, uint32_t pwmFreq)
{
    if (pwmFreq == 0) pwmFreq = 1;
    h->periodTicks = h->clkFreq / pwmFreq;
    DL_TimerG_setLoadValue(PWM_0_INST, h->periodTicks);
    /* Keep current duty (as a ratio) by reapplying current CCR proportion if you track it,
       or leave as-is and let app call PWM_setDuty() after changing freq. */
}

void PWM_start(PWM_Handle *h)
{
    (void)h;
    DL_TimerG_startCounter(PWM_0_INST);
}

void PWM_stop(PWM_Handle *h)
{
    (void)h;
    DL_TimerG_stopCounter(PWM_0_INST);
}


