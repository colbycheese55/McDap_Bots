// // main_pwm.c
// #include <stdint.h>
// #include "ti_msp_dl_config.h"      // brings in PWM_0_INST etc. + __NOP
// #include "LowLevelDrivers/inc/pwm.h"
#include "HighLevelDrivers/inc/motor.h"
// #include "OLED_Example.h"
// #include "LowLevelDrivers/inc/sleep.h"
// #include "Application/inc/ApplicationSwitcher.h"

// #include "hardware_init.h"

// // -------- Select which timer instances and CC indices you want to use --------
// // Example 1: both channels on the SAME timer instance (your current config)
// #define PWM_A_BASE   PWM_0_INST
// #define PWM_A_CCIDX  DL_TIMER_CC_0_INDEX   // CC0 (e.g., PA1 in your config)

// #define PWM_B_BASE   PWM_0_INST
// #define PWM_B_CCIDX  DL_TIMER_CC_1_INDEX   // CC1 (e.g., PA0 LED in your config)

// // Example 2 (uncomment to use different timers if you add PWM_1_INST later):
// // #define PWM_A_BASE   PWM_0_INST
// // #define PWM_A_CCIDX  DL_TIMER_CC_0_INDEX
// // #define PWM_B_BASE   PWM_1_INST
// // #define PWM_B_CCIDX  DL_TIMER_CC_0_INDEX

// // Timer clock (Hz). Your SysConfig sets BUSCLK = 32 MHz.
// #define TIMER_CLK_HZ   32000000u

// // static void delay_ms(uint32_t ms)
// // {
// //     volatile uint32_t cycles = ms * 1000u; // ~32 MHz -> ~1ms per 32000 loops
// //     while (cycles--) { __NOP(); }
// // }

int main(void)
{
    // SYSCFG_DL_init();  // pinmux + power + peripheral init (from SysConfig)
    hardware_init();

    // Run only the maze-bump roaming behavior from main
    run_maze_bump();

    // Ensure motors are stopped if the routine returns
    motor_set_speed_left(0);
    motor_set_speed_right(0);

    // OLED_Example_App();
    OLED_Example_App();
    run_application_switcher();


//     // while (1) {
//     //     drive_straight_distance(500, 0.3);
//     //     turn_left(90, 0.1);
//     //     drive_straight_distance(2000, 1);
//     //     turn_right_in_place(180, 0.2);
//     //     sleep_ms(3000);
//     // }
    


    

//     // Open two logical PWM channels using the generic driver
//     // PWM_Handle chA, chB;

//     // // Choose a human-visible PWM frequency for LED demos (1 kHz)
//     // const uint32_t pwmHz = 1000u;

//     // // Initialize both channels (can be same base or different bases)
//     // PWM_init(&chA, PWM_A_BASE, PWM_A_CCIDX, TIMER_CLK_HZ, pwmHz, 0.60f); // 60%
//     // PWM_init(&chB, PWM_B_BASE, PWM_B_CCIDX, TIMER_CLK_HZ, pwmHz, 0.30f); // 30%

//     // // Start both underlying timers. (If both channels use the same base,
//     // // starting twice is harmless.)
//     // PWM_start(&chA);
//     // PWM_start(&chB);
//     // // Start both underlying timers. (If both channels use the same base,
//     // // starting twice is harmless.)
//     // PWM_start(&chA);
//     // PWM_start(&chB);

//     // // ===== Case 1: BOTH ON =====
//     // // A = 60%, B = 30% (adjust to taste)
//     // PWM_setDuty(&chA, 0.60f);
//     // PWM_setDuty(&chB, 0.30f);
//     // delay_ms(1500);
//     // // ===== Case 1: BOTH ON =====
//     // // A = 60%, B = 30% (adjust to taste)
//     // PWM_setDuty(&chA, 0.60f);
//     // PWM_setDuty(&chB, 0.30f);
//     // delay_ms(1500);

//     // /* ===== Case 2: ONLY CC0 ON =====
//     //    Park CC1 by setting duty to 0% → PA0 LED steady/off.
//     //    CC0 keeps PWM (still not visible without a probe on PA1). */
//     // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.0f);   // PA0 steady
//     // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.70f);  // PA1 running (unseen)
//     // delay_ms(1500);

//     // /* ===== Case 3: ONLY CC1 ON =====
//     //    Park CC0 (0%), run CC1 (LED) at 80% → PA0 LED bright PWM. */
//     // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.0f);   // PA1 parked
//     // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.80f);  // PA0 LED PWM
//     // delay_ms(1500);

//     // /* Loop: repeat the three cases so you can observe on PA0 */

//     // int count = 0;
//     // while (1) {
//     //     // // both on
//     //     // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.50f);
//     //     // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.10f);  // PA0 visible
//     //     // delay_ms(1000);

//     //     // // only CC0 (PA0 steady)
//     //     // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.0f);
//     //     // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.70f);
//     //     // delay_ms(1000);

//     //     // // only CC1 (PA0 PWM)
//     //     // pwm_set_duty(DL_TIMER_CC_0_INDEX, load, 0.0f);
//     //     // pwm_set_duty(DL_TIMER_CC_1_INDEX, load, 0.50f);
//     //     // delay_ms(1000);

//     //     pwm_set_duty(DL_TIMER_CC_0_INDEX, load, (count % 100) / 100.0);
//     //     pwm_set_duty(DL_TIMER_CC_1_INDEX, load, (count % 100) / 100.0);
//     //     count += 1;
//     //     delay_ms(50);
//     // }
}











