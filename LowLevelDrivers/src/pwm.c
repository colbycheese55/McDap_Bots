#include "../inc/pwm.h"

// --------------------------------------------------------
//  Common helper
// --------------------------------------------------------
static uint32_t clamp_cc(uint32_t period, float duty) {
    if (duty < 0.f) duty = 0.f;
    if (duty > 1.f) duty = 1.f;
    uint32_t cmp = (uint32_t)(period * duty + 0.5f);
    if (cmp > period) cmp = period;
    return cmp;
}

static inline void set_cc(PWM_Handle *h, uint32_t cmp) {
    DL_TimerG_setCaptureCompareValue((GPTIMER_Regs*)h->peripheral,
                                     cmp,
                                     h->ccChannel);
}

void PWM_init(PWM_Handle *h,
              void *peripheral,
              uint8_t ccChannel,
              uint32_t clkHz,
              uint32_t pwmHz,
              float duty)
{
    if (pwmHz == 0) pwmHz = 1;

    h->peripheral = peripheral;
    h->ccChannel  = ccChannel;
    h->clkHz      = clkHz;
    h->period     = clkHz / pwmHz;
    h->duty       = duty;

    DL_TimerG_setLoadValue((GPTIMER_Regs*)peripheral, h->period);
    set_cc(h, clamp_cc(h->period, duty));
}

void PWM_setDuty(PWM_Handle *h, float duty) {
    h->duty = duty;
    set_cc(h, clamp_cc(h->period, duty));
}

void PWM_setFrequency(PWM_Handle *h, uint32_t pwmHz) {
    if (pwmHz == 0) pwmHz = 1;
    h->period = h->clkHz / pwmHz;

    DL_TimerG_setLoadValue((GPTIMER_Regs*)h->peripheral, h->period);
    PWM_setDuty(h, h->duty);
}

// void PWM_stop(PWM_Handle *h)
// {
//     (void)h;
//     DL_TimerG_stopCounter(PWM_0_INST);
// }

