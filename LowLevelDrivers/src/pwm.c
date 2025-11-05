#include "../inc/pwm.h"

static inline void set_cc(const PWM_Handle *h, uint32_t cmp) {
    DL_TimerG_setCaptureCompareValue(h->base, cmp, h->ccIndex);
}

void PWM_init(PWM_Handle *h,
              GPTIMER_Regs *base,     // <-- correct type
              uint8_t ccIndex,
              uint32_t clkHz,
              uint32_t pwmHz,
              float duty)
{
    if (pwmHz == 0) pwmHz = 1;
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;

    h->base    = base;
    h->ccIndex = ccIndex;
    h->clkHz   = clkHz;
    h->period  = clkHz / pwmHz;
    h->duty    = duty;

    DL_TimerG_setLoadValue(h->base, h->period);

    uint32_t cmp = (uint32_t)((float)h->period * duty + 0.5f);
    if (cmp > h->period) cmp = h->period;
    set_cc(h, cmp);
}

void PWM_setDuty(PWM_Handle *h, float duty)
{
    if (duty < 0.0f) duty = 0.0f;
    if (duty > 1.0f) duty = 1.0f;
    h->duty = duty;

    uint32_t cmp = (uint32_t)((float)h->period * duty + 0.5f);
    if (cmp > h->period) cmp = h->period;
    set_cc(h, cmp);
}

void PWM_setFrequency(PWM_Handle *h, uint32_t pwmHz)
{
    if (pwmHz == 0) pwmHz = 1;
    h->period = h->clkHz / pwmHz;
    DL_TimerG_setLoadValue(h->base, h->period);
    PWM_setDuty(h, h->duty);
}

// void PWM_stop(PWM_Handle *h)
// {
//     (void)h;
//     DL_TimerG_stopCounter(PWM_0_INST);
// }

