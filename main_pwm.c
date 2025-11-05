// main_pwm.c  (generic demo for multiple PWM instances)
#include <stdint.h>
#include "ti_msp_dl_config.h"      // brings in PWM_0_INST etc. + __NOP
#include "LowLevelDrivers/inc/pwm.h"

// -------- Select which timer instances and CC indices you want to use --------
// Example 1: both channels on the SAME timer instance (your current config)
#define PWM_A_BASE   PWM_0_INST
#define PWM_A_CCIDX  DL_TIMER_CC_0_INDEX   // CC0 (e.g., PA1 in your config)

#define PWM_B_BASE   PWM_0_INST
#define PWM_B_CCIDX  DL_TIMER_CC_1_INDEX   // CC1 (e.g., PA0 LED in your config)

// Example 2 (uncomment to use different timers if you add PWM_1_INST later):
// #define PWM_A_BASE   PWM_0_INST
// #define PWM_A_CCIDX  DL_TIMER_CC_0_INDEX
// #define PWM_B_BASE   PWM_1_INST
// #define PWM_B_CCIDX  DL_TIMER_CC_0_INDEX

// Timer clock (Hz). Your SysConfig sets BUSCLK = 32 MHz.
#define TIMER_CLK_HZ   32000000u

static void delay_ms(uint32_t ms)
{
    volatile uint32_t cycles = ms * 1000u; // ~1ms per 32k loops at 32 MHz (rough)
    while (cycles--) { __NOP(); }
}

int main(void)
{
    SYSCFG_DL_init();  // pinmux + power + peripheral init (from SysConfig)

    // Open two logical PWM channels using the generic driver
    PWM_Handle chA, chB;

    // Choose a human-visible PWM frequency for LED demos (1 kHz)
    const uint32_t pwmHz = 1000u;

    // Initialize both channels (can be same base or different bases)
    PWM_init(&chA, PWM_A_BASE, PWM_A_CCIDX, TIMER_CLK_HZ, pwmHz, 0.60f); // 60%
    PWM_init(&chB, PWM_B_BASE, PWM_B_CCIDX, TIMER_CLK_HZ, pwmHz, 0.30f); // 30%

    // Start both underlying timers. (If both channels use the same base,
    // starting twice is harmless.)
    PWM_start(&chA);
    PWM_start(&chB);

    // ===== Case 1: BOTH ON =====
    // A = 60%, B = 30% (adjust to taste)
    PWM_setDuty(&chA, 0.60f);
    PWM_setDuty(&chB, 0.30f);
    delay_ms(1500);

    // ===== Case 2: ONLY A ON =====
    // Park B by setting duty to 0% (steady level). A keeps PWM’ing.
    PWM_setDuty(&chB, 0.0f);
    PWM_setDuty(&chA, 0.70f);
    delay_ms(1500);

    // ===== Case 3: ONLY B ON =====
    // Park A; run B.
    PWM_setDuty(&chA, 0.0f);
    PWM_setDuty(&chB, 0.80f);
    delay_ms(1500);

    // Loop through the 3 behaviors so you can observe easily
    while (1) {
        // both on
        PWM_setDuty(&chA, 0.50f);
        PWM_setDuty(&chB, 0.0f);
        delay_ms(1000);

        // only A
        PWM_setDuty(&chB, 0.0f);
        PWM_setDuty(&chA, 0.70f);
        delay_ms(1000);

        // only B
        PWM_setDuty(&chA, 0.0f);
        PWM_setDuty(&chB, 0.80f);
        delay_ms(1000);
    }
}












