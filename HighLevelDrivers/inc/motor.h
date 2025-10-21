#ifndef MOTOR_H
#define MOTOR_H

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
    PWM_Handle* left_motor_forward;
    PWM_Handle* left_motor_backward;
    PWM_Handle* right_motor_forward;
    PWM_Handle* right_motor_backward;

    GPIO_Regs* encoder_a_port;
    uint32_t encoder_a_pin;
    GPIO_Regs* encoder_b_port;
    uint32_t encoder_b_pin;
} Motor_Handle;

/* ------------------------------------------------------------------ */
/* Basic functions                                                    */
/* ------------------------------------------------------------------ */
/**
 * @brief Initialize motor subsystem and hardware handles.
 *
 * @param motor Pointer to a Motor_Handle structure. The fields must be
 *              populated by the caller with valid PWM handles and GPIO
 *              port/pin pairs before calling this function.
 */
void motor_init(Motor_Handle* motor);

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

/* ------------------------------------------------------------------ */
/* Higher-level motion primitives                                     */
/* ------------------------------------------------------------------ */
/**
 * @brief Drive straight for a given distance.
 *
 * This is a blocking call that returns once the robot has driven the
 * requested linear distance.
 *
 * @param distance Distance to travel in millimeters. Must be positive.
 * @param speed    Speed in the range [-1.0, 1.0]. Direction is implied
 *                 by the sign; positive values move forward.
 */
void drive_straight_distance(float distance, float speed);

/**
 * @brief Perform a non-zero-radius left turn.
 *
 * This function blocks until the turn is complete.
 *
 * @param angle Angle in degrees (positive). Robot will turn left by this
 *              amount while moving at the provided speed.
 * @param speed Speed in the range [-1.0, 1.0].
 */
void turn_left(float angle, float speed);

/**
 * @brief Perform a non-zero-radius right turn.
 *
 * This function blocks until the turn is complete.
 *
 * @param angle Angle in degrees (positive). Robot will turn right by this
 *              amount while moving at the provided speed.
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
 * Rotate the robot about its center by driving the wheels in opposite
 * directions. This call blocks until the requested angular displacement
 * has been reached.
 *
 * @param angle Angle in degrees (positive).
 * @param speed Speed magnitude in the range [0.0, 1.0]; direction is
 *              determined by the function (right in-place rotation).
 */
void turn_right_in_place(float angle, float speed);

#endif // MOTOR_H