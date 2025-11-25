#include "adc.h"
#include "gpio.h"
#include "i2c.h"
#include "pwm.h"
#include "uart.h"

#include "bumpSwitches.h"
#include "IRSensor.h"
#include "motor.h"
#include "SSD1306.h"
#include "reflectance_sensor.h"

#include "ApplicationSwitcher.h"

#include "stm32g0xx_hal.h"

#include "main.h"

extern ADC_HandleTypeDef hadc1;

extern I2C_HandleTypeDef hi2c1;

extern TIM_HandleTypeDef htim1;
extern TIM_HandleTypeDef htim2;

extern UART_HandleTypeDef huart2;



void hardware_init(void)
{

    // Initialize I2C devices (OLED screen controller and reflectance sensor's GPIO expander)
    I2C_Handle i2c = {
        .inst = &hi2c1
    };
    SSD1306_Init(i2c, SSD1306_SWITCHCAPVCC);
    // refsen_init(&i2c);


    // bump switches
    GPIO_Handle bump_far_left = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_3
    };
    GPIO_Handle bump_mid_left = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_0
    };
    GPIO_Handle bump_mid_right = {
        .peripheral = GPIOA,
        .pin = GPIO_PIN_6
    };
    GPIO_Handle bump_far_right = {
        .peripheral = GPIOA,
        .pin = GPIO_PIN_7
    };
    bump_switches_init(bump_far_left, bump_mid_left, bump_mid_right, bump_far_right);


    // IR sensors
    ADC_Handle ir_left = {
        .hadc = &hadc1,
		.channel = ADC_CHANNEL_3
    };
    ADC_Handle ir_center = {
    	.hadc = &hadc1,
    	.channel = ADC_CHANNEL_0
    };
    ADC_Handle ir_right = {
    	.hadc = &hadc1,
    	.channel = ADC_CHANNEL_1
    };
    ir_init(ir_left, ir_center, ir_right);


    // motors
    PWM_Handle motor_left_m1;
    PWM_Handle motor_left_m2;
    PWM_Handle motor_right_m1;
    PWM_Handle motor_right_m2;
    uint32_t clkHz = 16000000; // MCU clock frequency
    uint32_t pwmHz = 1000;
    PWM_init(&motor_left_m1,
             &htim2,
			 TIM_CHANNEL_2,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_left_m2,
    		 &htim2,
			 TIM_CHANNEL_3,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_right_m1,
    		 &htim1,
			 TIM_CHANNEL_3,
             clkHz,
             pwmHz,
             0.0f);
    PWM_init(&motor_right_m2,
    		 &htim1,
			 TIM_CHANNEL_1,
             clkHz,
             pwmHz,
             0.0f);

    GPIO_Handle motor_left_a = {
        .peripheral = GPIOB,
        .pin = GPIO_PIN_15
    };
    GPIO_Handle motor_left_b = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_2
    };
    GPIO_Handle motor_right_a = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_1
    };
    GPIO_Handle motor_right_b = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_4
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
    GPIO_Handle user_button = {
        .peripheral = GPIOC,
        .pin = GPIO_PIN_13
    };
    GPIO_Handle user_LED = {
        .peripheral = GPIOA,
        .pin = GPIO_PIN_5
    };
}
