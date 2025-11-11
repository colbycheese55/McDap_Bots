/**
 * @file motor.h
 * @brief High level motor control API
 *
 * Declarations for motor control routines and the Motor_Handle structure.
 */
#ifndef MOTOR_H
#define MOTOR_H

#include <stdint.h>
#include "../../LowLevelDrivers/inc/gpio.h"
#include "../../LowLevelDrivers/inc/pwm.h"

/**
 * @brief Motor handle containing PWM channels and encoder pin mappings.
 *
 * The caller should populate this structure with valid PWM handles for
 * forward/backward control of each motor and the GPIO port/pin for
 * the wheel encoders.
 */
typedef struct {
    PWM_Handle left_motor_forward;
    PWM_Handle left_motor_backward;
    PWM_Handle right_motor_forward;
    PWM_Handle right_motor_backward;

    GPIO_Handle left_encoder_a;
    GPIO_Handle left_encoder_b;
    GPIO_Handle right_encoder_a;
    GPIO_Handle right_encoder_b;
} Motor_Handle;


/**
 * @brief Initialize motor subsystem and hardware handles.
 *
 * @param motor_handle A Motor_Handle structure. The fields must be
 *                     populated by the caller with valid PWM handles and GPIO
 *                     port/pin pairs before calling this function.
 */
void motor_init(Motor_Handle motor_handle);

/**
 * @brief Set the speed of the left motor.
 *
 * @param speed Motor speed in the range [-1.0, 1.0]. Negative values
 *              indicate reverse direction, positive values forward.
 */
void motor_set_speed_left(float speed);

/**
 * @brief Set the speed of the right motor.
 *
 * @param speed Motor speed in the range [-1.0, 1.0]. Negative values
 *              indicate reverse direction, positive values forward.
 */
void motor_set_speed_right(float speed);

/**
 * @brief Drive in an arc while moving.
 *
 * Drives the robot in an arc defined by the translational and angular
 * speeds. Positive angular speed indicates a left turn, negative
 * indicates a right turn.
 *
 * @param translational_speed Speed in the range [-1.0, 1.0]. Direction is
 *                             implied by the sign; positive moves forward.
 * @param angular_speed         Angular speed in the range [-1.0, 1.0].
 */
void drive_arc(float translational_speed, float angular_speed);

/**
 * @brief Drive straight for a given distance (blocking).
 *
 * Drives the robot forward (or backward if speed negative) until the
 * requested linear distance has been traveled.
 *
 * @param distance Distance to travel in millimeters. Must be positive.
 * @param speed    Speed in the range [-1.0, 1.0]. Direction is implied
 *                 by the sign; positive moves forward.
 */
void drive_straight_distance(float distance, float speed);

/**
 * @brief Perform a non-zero-radius left turn (arc) while driving (blocking).
 *
 * @param angle Angle in degrees (positive) to turn left.
 * @param speed Speed in the range [-1.0, 1.0].
 */
void turn_left(float angle, float speed);

/**
 * @brief Perform a non-zero-radius right turn (arc) while driving (blocking).
 *
 * @param angle Angle in degrees (positive) to turn right.
 * @param speed Speed in the range [-1.0, 1.0].
 */
void turn_right(float angle, float speed);

/**
 * @brief Rotate the robot in place to the left (spin in place).
 *
 * Rotate the robot about its center by driving the wheels in opposite
 * directions. This call blocks until the requested angular displacement
 * has been reached.
 *
 * @param angle Angle in degrees (positive).
 * @param speed Speed magnitude in the range [0.0, 1.0]; direction is
 *              determined by the function (left in-place rotation).
 */
void turn_left_in_place(float angle, float speed);

/**
 * @brief Rotate the robot in place to the right (spin in place).
 *
 * @param angle Angle in degrees (positive).
 * @param speed Speed magnitude in the range [0.0, 1.0]; direction is
 *              determined by the function (right in-place rotation).
 */
void turn_right_in_place(float angle, float speed);

#endif // MOTOR_H
