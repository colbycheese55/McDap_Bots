#include "pwm.h"


// BASIC FUNCTIONS
// initialize the motors with the given PWM handles
void motor_init(PWM_Handle* left_forward, PWM_Handle* left_backward, PWM_Handle* right_forward, PWM_Handle* right_backward);

// set the speed of the left motor (-1.0 to 1.0)
void motor_set_speed_left(float speed);

// set the speed of the right motor (-1.0 to 1.0)
void motor_set_speed_right(float speed);

// ADVANCED FUNCTIONS
// todo