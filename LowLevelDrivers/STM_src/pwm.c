#include "pwm.h"
#include "stm32g0xx_hal.h"

static uint32_t clamp_cc(uint32_t period, float duty) {
    if (duty < 0.f) duty = 0.f;
    if (duty > 1.f) duty = 1.f;
    uint32_t cmp = (uint32_t)(period * duty + 0.5f);
    if (cmp > period) cmp = period;
    return cmp;
}

static inline void set_cc(PWM_Handle *h, uint32_t cmp) {
    __HAL_TIM_SET_COMPARE((TIM_HandleTypeDef*)h->peripheral,
                          h->ccChannel,
                          cmp);
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

    // Set ARR
    __HAL_TIM_SET_AUTORELOAD((TIM_HandleTypeDef*)peripheral, h->period);

    // Set CCR
    set_cc(h, clamp_cc(h->period, duty));
}

void PWM_start(PWM_Handle *h) {
    HAL_TIM_PWM_Start((TIM_HandleTypeDef *)h->peripheral, h->ccChannel);
}

void PWM_stop(PWM_Handle *h) {
    HAL_TIM_PWM_Stop((TIM_HandleTypeDef *)h->peripheral, h->ccChannel);
}

void PWM_setDuty(PWM_Handle *h, float duty) {
    h->duty = duty;
    set_cc(h, clamp_cc(h->period, duty));
}

void PWM_setFrequency(PWM_Handle *h, uint32_t pwmHz) {
    if (pwmHz == 0) pwmHz = 1;
    h->period = h->clkHz / pwmHz;

    __HAL_TIM_SET_AUTORELOAD((TIM_HandleTypeDef*)h->peripheral, h->period);
    PWM_setDuty(h, h->duty);
}
