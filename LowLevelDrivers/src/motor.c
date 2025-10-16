#include "../inc/motor.h"

PWM_Handle* left_motor_forward;
PWM_Handle* left_motor_backward;
PWM_Handle* right_motor_forward;
PWM_Handle* right_motor_backward;

void motor_init(PWM_Handle* left_forward, PWM_Handle* left_backward, PWM_Handle* right_forward, PWM_Handle* right_backward) {
    left_motor_forward = left_forward;
    left_motor_backward = left_backward;
    right_motor_forward = right_forward;
    right_motor_backward = right_backward;

    PWM_start(left_motor_forward);
    PWM_start(left_motor_backward);
    PWM_start(right_motor_forward);
    PWM_start(right_motor_backward);
}

void motor_set_speed_left(float speed) {
    if (speed >= 0) {
        PWM_setDuty(left_motor_forward, speed);
        PWM_setDuty(left_motor_backward, 0);
    } 
    else {
        PWM_setDuty(left_motor_forward, 0);
        PWM_setDuty(left_motor_backward, -speed);
    }
}

void motor_set_speed_right(float speed) {
    if (speed >= 0) {
        PWM_setDuty(right_motor_forward, speed);
        PWM_setDuty(right_motor_backward, 0);
    } 
    else {
        PWM_setDuty(right_motor_forward, 0);
        PWM_setDuty(right_motor_backward, -speed);
    }
}