#ifndef PWM_H_
#define PWM_H_

#include "ti_msp_dl_config.h"        // brings in msp.h + dl_core.h
#include <ti/driverlib/dl_timer.h>   // DL_TimerG_* APIs + GPTIMER_Regs

typedef struct {
    GPTIMER_Regs *base;   // <-- correct type
    uint8_t       ccIndex; // DL_TIMER_CC_0_INDEX or DL_TIMER_CC_1_INDEX
    uint32_t      clkHz;
    uint32_t      period;
    float         duty;    // 0.0..1.0
} PWM_Handle;

void PWM_init(PWM_Handle *h,
              GPTIMER_Regs *base,
              uint8_t ccIndex,
              uint32_t clkHz,
              uint32_t pwmHz,
              float duty);

void PWM_setDuty(PWM_Handle *h, float duty);
void PWM_setFrequency(PWM_Handle *h, uint32_t pwmHz);

static inline void PWM_start(PWM_Handle *h) { DL_TimerG_startCounter(h->base); }
static inline void PWM_stop (PWM_Handle *h) { DL_TimerG_stopCounter(h->base);  }

static inline void PWM_disableChannel(PWM_Handle *h) { PWM_setDuty(h, 0.0f); }
static inline void PWM_enableChannel (PWM_Handle *h) { PWM_setDuty(h, h->duty); }

#endif /* PWM_H_ */

