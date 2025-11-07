#include "LowLevelDrivers/inc/adc.h"
#include "LowLevelDrivers/inc/gpio.h"
#include "LowLevelDrivers/inc/i2c.h"
#include "LowLevelDrivers/inc/pwm.h"
#include "LowLevelDrivers/inc/uart.h"

#include "HighLevelDrivers/inc/bumpSwitches.h"
#include "HighLevelDrivers/inc/IRSensor.h"

#include "ti_msp_dl_config.h"


void hardware_init(void)
{
    SYSCFG_DL_init();

    // I2C bus
    i2c_init(I2C_BUS_INST, 50, 51);

    // bump switches
    GPIO_Handle bump_far_left = {
        .peripheral = BUMP_SWITCHES_BS0_PORT,
        .pin = BUMP_SWITCHES_BS0_PIN
    };
    GPIO_Handle bump_mid_left = {
        .peripheral = BUMP_SWITCHES_BS1_PORT,
        .pin = BUMP_SWITCHES_BS1_PIN
    };
    GPIO_Handle bump_mid_right = {
        .peripheral = BUMP_SWITCHES_BS2_PORT,
        .pin = BUMP_SWITCHES_BS2_PIN
    };
    GPIO_Handle bump_far_right = {
        .peripheral = BUMP_SWITCHES_BS3_PORT,
        .pin = BUMP_SWITCHES_BS3_PIN
    };
    bump_switches_init(bump_far_left, bump_mid_left, bump_mid_right, bump_far_right);

    // IR sensors
    ADC_Handle ir_left = {
        .adc12 = IR_SENSORS_INST,
        .channel = IR_SENSORS_ADCMEM_0
    };
    ADC_Handle ir_center = {
        .adc12 = IR_SENSORS_INST,
        .channel = IR_SENSORS_ADCMEM_1
    };
    ADC_Handle ir_right = {
        .adc12 = IR_SENSORS_INST,
        .channel = IR_SENSORS_ADCMEM_2
    };
    ir_init(ir_left, ir_center, ir_right);

    // motors
    // PWM TODO
    GPIO_Handle motor_left_a = {
        .peripheral = MOTOR_ENCODERS_MOTOR_LEFT_A_PORT,
        .pin = MOTOR_ENCODERS_MOTOR_LEFT_A_PIN
    };
    GPIO_Handle motor_left_b = {
        .peripheral = MOTOR_ENCODERS_MOTOR_LEFT_B_PORT,
        .pin = MOTOR_ENCODERS_MOTOR_LEFT_B_PIN
    };
    GPIO_Handle motor_right_a = {
        .peripheral = MOTOR_ENCODERS_MOTOR_RIGHT_A_PORT,
        .pin = MOTOR_ENCODERS_MOTOR_RIGHT_A_PIN
    };
    GPIO_Handle motor_right_b = {
        .peripheral = MOTOR_ENCODERS_MOTOR_RIGHT_B_PORT,
        .pin = MOTOR_ENCODERS_MOTOR_RIGHT_B_PIN
    };
}