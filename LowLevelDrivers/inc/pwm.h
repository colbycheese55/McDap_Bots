#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>

#ifdef STM32G071xx
    #include "stm32g0xx_hal.h"
#elif __MSPM0G3507__
    #include "ti_msp_dl_config.h"        // brings in msp.h + dl_core.h
    #include <ti/driverlib/dl_timer.h>   // DL_TimerG_* APIs + GPTIMER_Regs
#endif //STM32G071xx



typedef struct {
    void     *peripheral;  // Timer base pointer (TIM_TypeDef* or GPTIMER_Regs*)
    uint8_t   ccChannel;   // CC index (TIM_CHANNEL_1, TI DL_TIMER_CC_0_INDEX…)
    uint32_t  clkHz;
    uint32_t  period;
    float     duty;
} PWM_Handle;

void PWM_init(PWM_Handle *h,
              void *peripheral,
              uint8_t ccChannel,
              uint32_t clkHz,
              uint32_t pwmHz,
              float duty);

void PWM_setDuty(PWM_Handle *h, float duty);
void PWM_setFrequency(PWM_Handle *h, uint32_t pwmHz);
void PWM_start(PWM_Handle *h);
void PWM_stop(PWM_Handle *h);

static inline void PWM_disableChannel(PWM_Handle *h) { PWM_setDuty(h, 0.0f); }
static inline void PWM_enableChannel (PWM_Handle *h) { PWM_setDuty(h, h->duty); }

#endif /* PWM_H_ */

