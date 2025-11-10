#include "../inc/motor.h"
#include "../../LowLevelDrivers/inc/gpio.h"
#include "../../LowLevelDrivers/inc/pwm.h"

#define QUADS_PER_REV   (360 * 4)
#define WHEEL_DIAMETER  70    // in mm
#define ROBOT_WIDTH     146   // in mm

Motor_Handle motor;
uint8_t quads_state_left[2] = {0, 0};
uint8_t quads_state_right[2] = {0, 0};

void motor_init(Motor_Handle motor_handle) {
    motor = motor_handle;

    PWM_start(&(motor.left_motor_forward));
    PWM_start(&(motor.left_motor_backward));
    PWM_start(&(motor.right_motor_forward));
    PWM_start(&(motor.right_motor_backward));
}

void motor_set_speed_left(float speed) {
    if (speed >= 0) {
        PWM_setDuty(&(motor.left_motor_forward), speed);
        PWM_setDuty(&(motor.left_motor_backward), 0);
    } 
    else {
        PWM_setDuty(&(motor.left_motor_forward), 0);
        PWM_setDuty(&(motor.left_motor_backward), -speed);
    }
}

void motor_set_speed_right(float speed) {
    if (speed >= 0) {
        PWM_setDuty(&(motor.right_motor_forward), 0);
        PWM_setDuty(&(motor.right_motor_backward), speed);
    } 
    else {
        PWM_setDuty(&(motor.right_motor_forward), -speed);
        PWM_setDuty(&(motor.right_motor_backward), 0);
    }
}

// helper function
void wait_on_quads_left(uint32_t quads) {
    uint32_t count = 0;
    while (count < quads) {
        uint8_t a = gpio_read_pin(motor.left_encoder_a);
        uint8_t b = gpio_read_pin(motor.left_encoder_b);
        if (a != quads_state_left[0] || b != quads_state_left[1]) {
            quads_state_left[0] = a;
            quads_state_left[1] = b;
            count++;
        }
    }
}

void wait_on_quads_right(uint32_t quads) {
    uint32_t count = 0;
    while (count < quads) {
        uint8_t a = gpio_read_pin(motor.right_encoder_a);
        uint8_t b = gpio_read_pin(motor.right_encoder_b);
        if (a != quads_state_right[0] || b != quads_state_right[1]) {
            quads_state_right[0] = a;
            quads_state_right[1] = b;
            count++;
        }
    }
}

void drive_straight_distance(float distance, float speed) {
    motor_set_speed_left(speed);
    motor_set_speed_right(speed);

    float quads_needed = (distance / (3.14159 * WHEEL_DIAMETER)) * QUADS_PER_REV;
    wait_on_quads_left((int)quads_needed);

    motor_set_speed_left(0);
    motor_set_speed_right(0);
}

void turn_left(float angle, float speed) {
    motor_set_speed_left(0);
    motor_set_speed_right(speed);

    float turn_circumference = 3.14159 * ROBOT_WIDTH * 2 * (angle / 360.0f);
    float quads_needed = (turn_circumference / (3.14159 * WHEEL_DIAMETER)) * QUADS_PER_REV;
    wait_on_quads_right((int)quads_needed);

    motor_set_speed_left(0);
    motor_set_speed_right(0);
}

void turn_right(float angle, float speed) {
    motor_set_speed_left(speed);
    motor_set_speed_right(0);

    float turn_circumference = 3.14159 * ROBOT_WIDTH * 2 * (angle / 360.0f);
    float quads_needed = (turn_circumference / (3.14159 * WHEEL_DIAMETER)) * QUADS_PER_REV;
    wait_on_quads_left((int)quads_needed);

    motor_set_speed_left(0);
    motor_set_speed_right(0);
}

void turn_left_in_place(float angle, float speed) {
    motor_set_speed_left(-speed);
    motor_set_speed_right(speed);

    float turn_circumference = 3.14159 * ROBOT_WIDTH * (angle / 360.0f);
    float quads_needed = (turn_circumference / (3.14159 * WHEEL_DIAMETER)) * QUADS_PER_REV;
    wait_on_quads_left((int)quads_needed);

    motor_set_speed_left(0);
    motor_set_speed_right(0);
}

void turn_right_in_place(float angle, float speed) {
    motor_set_speed_left(speed);
    motor_set_speed_right(-speed);

    float turn_circumference = 3.14159 * ROBOT_WIDTH * (angle / 360.0f);
    float quads_needed = (turn_circumference / (3.14159 * WHEEL_DIAMETER)) * QUADS_PER_REV;
    wait_on_quads_right((int)quads_needed);

    motor_set_speed_left(0);
    motor_set_speed_right(0);
}