#include "LowLevelDrivers/inc/adc.h"
#include "LowLevelDrivers/inc/gpio.h"
#include "LowLevelDrivers/inc/i2c.h"
#include "LowLevelDrivers/inc/pwm.h"
#include "LowLevelDrivers/inc/uart.h"

#include "HighLevelDrivers/inc/bumpSwitches.h"
#include "HighLevelDrivers/inc/IRSensor.h"
#include "HighLevelDrivers/inc/motor.h"
#include "HighLevelDrivers/inc/SSD1306.h"
#include "HighLevelDrivers/inc/reflectance_sensor.h"

#include "Application/inc/ApplicationSwitcher.h"


#include "ti_msp_dl_config.h"


void hardware_init(void)
{
    SYSCFG_DL_init();

    // Initialize I2C devices (OLED screen controller and reflectance sensor's GPIO expander)
    I2C_Handle i2c = {
        .inst = I2C1
    };
    SSD1306_Init(i2c, SSD1306_SWITCHCAPVCC);
    // refsen_init(&i2c);
    

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
        .channel = IR_SENSORS_ADCMEM_0,
        .mem_result_loaded = DL_ADC12_IIDX_MEM2_RESULT_LOADED // use last ADC conversion memory in sequence
    };
    ADC_Handle ir_center = {
        .adc12 = IR_SENSORS_INST,
        .channel = IR_SENSORS_ADCMEM_1,
        .mem_result_loaded = DL_ADC12_IIDX_MEM2_RESULT_LOADED // use last ADC conversion memory in sequence
    };
    ADC_Handle ir_right = {
        .adc12 = IR_SENSORS_INST,
        .channel = IR_SENSORS_ADCMEM_2,
        .mem_result_loaded = DL_ADC12_IIDX_MEM2_RESULT_LOADED // use last ADC conversion memory in sequence
    };
    NVIC_EnableIRQ(IR_SENSORS_INST_INT_IRQN); // Enable interrupt requests for ADC
    ir_init(ir_left, ir_center, ir_right);

    
    // motors
    PWM_Handle motor_left_m1;
    PWM_Handle motor_left_m2;
    PWM_Handle motor_right_m1;
    PWM_Handle motor_right_m2;
    uint32_t clkHz = 32000000; // MCU clock frequency
    uint32_t pwmHz = 10000;
    PWM_init(&motor_left_m1,
             MOTOR_LEFT_RIGHT_M1_INST,
             GPIO_MOTOR_LEFT_RIGHT_M1_C0_IDX,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_left_m2,
             MOTOR_LEFT_M2_INST,
             GPIO_MOTOR_LEFT_M2_C2_IDX,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_right_m1,
             MOTOR_LEFT_RIGHT_M1_INST,
             GPIO_MOTOR_LEFT_RIGHT_M1_C1_IDX,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_right_m2,
             MOTOR_RIGHT_M2_INST,
             GPIO_MOTOR_RIGHT_M2_C1_IDX,
             clkHz,
             pwmHz,
             0.0f);

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

    Motor_Handle motors = {
        .left_motor_forward = motor_left_m1,
        .left_motor_backward = motor_left_m2,
        .right_motor_forward = motor_right_m1,
        .right_motor_backward = motor_right_m2,
        .left_encoder_a = motor_left_a,
        .left_encoder_b = motor_left_b,
        .right_encoder_a = motor_right_a,
        .right_encoder_b = motor_right_b
    };
    motor_init(motors);

    // MSPM user buttons
    GPIO_Handle user_button_1 = {
        .peripheral = MSPM0_USER_BTN1_PORT,
        .pin = MSPM0_USER_BTN1_PIN
    };
    GPIO_Handle user_button_2 = {
        .peripheral = MSPM0_USER_BTN2_PORT,
        .pin = MSPM0_USER_BTN2_PIN
    };
    NVIC_EnableIRQ(MSPM0_USER_GPIOA_INT_IRQN);
    NVIC_EnableIRQ(MSPM0_USER_GPIOB_INT_IRQN);
}


// // user button interrupts
void GROUP1_IRQHandler(void)
{
    application_yield = true;
}