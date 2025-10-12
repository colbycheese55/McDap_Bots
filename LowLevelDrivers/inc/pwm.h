#ifndef PWM_H_
#define PWM_H_

#include <stdint.h>
#include "ti_msp_dl_config_PWM.h"

/* Pull in SysConfig + DriverLib + base addresses + CMSIS NOP, etc.
 * This hides all those dependencies from main.c.
 */

/* Channel mapping: 0 = CCP0 (PA1), 1 = CCP1 (PA0 / red LED via J4) */
typedef struct {
    uint8_t  channel;
    uint32_t clkFreq;
    uint32_t periodTicks;
} PWM_Handle;

/* Board/system init (wraps SYSCFG_DL_init). Call this once at boot. */
void PWM_boardInit(void);

/* Initialize PWM on PWM_0_INST with frequency and duty cycle */
void PWM_init(PWM_Handle *h, uint8_t channel,
              uint32_t clkFreq, uint32_t pwmFreq, float duty);

/* Update duty cycle (0.0 – 1.0) */
void PWM_setDuty(PWM_Handle *h, float duty);

/* Update PWM frequency (Hz) */
void PWM_setFrequency(PWM_Handle *h, uint32_t pwmFreq);

/* Start/stop timer */
void PWM_start(PWM_Handle *h);
void PWM_stop(PWM_Handle *h);

#endif /* PWM_H_ */
