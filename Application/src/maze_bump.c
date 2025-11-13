#include <stdint.h>
#include <stdbool.h>

#include "../inc/maze_bump.h"
#include "../inc/ApplicationSwitcher.h" // for application_yield (extern)
#include "../../HighLevelDrivers/inc/motor.h"
#include "../../HighLevelDrivers/inc/bumpSwitches.h"
#include "../../HighLevelDrivers/inc/IRSensor.h"
#include "../../LowLevelDrivers/inc/sleep.h"

// Tunable parameters
#define FORWARD_SPEED        0.4f
#define SLOW_SPEED           0.2f
#define SLOW_DISTANCE_MM     120u   // start slowing when this close
#define STOP_DISTANCE_MM     60u    // stop if too close
#define BACKUP_DISTANCE_MM   80.0f  // mm to back up after bump
#define BACKUP_SPEED         0.3f
#define TURN_MIN_DEG         45u
#define TURN_MAX_DEG         140u
#define TURN_SPEED           0.25f
#define DEBOUNCE_MS          40u
#define POLL_MS              30u

// Application switcher declares this symbol as a global; reference it.
extern volatile bool application_yield;

// Small LFSR PRNG (32-bit) for lightweight randomness
static uint32_t lfsr = 0xACE1u;
static uint32_t next_rand(void) {
    // xorshift-ish LFSR
    lfsr ^= lfsr << 13;
    lfsr ^= lfsr >> 17;
    lfsr ^= lfsr << 5;
    return lfsr;
}

static bool any_bump_pressed(void) {
    return bump_switch_read(FAR_LEFT) || bump_switch_read(MID_LEFT) || bump_switch_read(MID_RIGHT) || bump_switch_read(FAR_RIGHT);
}

void run_maze_bump(void) {
    // Basic roaming loop: drive forward, slow on IR, debounce bumps, backup, random turn
    while (!application_yield) {
        // Read center IR
        uint8_t dist = SLOW_DISTANCE_MM - 10u;// ir_get_distance(CENTER);

        // If very close, stop and wait a moment
        if (dist <= STOP_DISTANCE_MM) {
            motor_set_speed_left(0.0f);
            motor_set_speed_right(0.0f);
            // short pause to avoid hammering
            sleep_ms(50);
            // continue so we re-evaluate sensor
            if (application_yield) break;
            continue;
        }

        // Scale speed based on distance (linear taper between SLOW_DISTANCE and farther)
        float speed = FORWARD_SPEED;
        if (dist < SLOW_DISTANCE_MM) {
            float ratio = (float)dist / (float)SLOW_DISTANCE_MM;
            if (ratio < 0.0f) ratio = 0.0f;
            if (ratio > 1.0f) ratio = 1.0f;
            // Interpolate between SLOW_SPEED (close) and FORWARD_SPEED (far)
            speed = SLOW_SPEED + (FORWARD_SPEED - SLOW_SPEED) * ratio;
        }

        motor_set_speed_left(speed);
        motor_set_speed_right(speed);

        // Poll bump switches, debounce briefly
        if (any_bump_pressed()) {
            // small debounce window
            sleep_ms(DEBOUNCE_MS);
            if (!any_bump_pressed()) {
                // false alarm
                continue;
            }

            // Confirmed bump
            motor_set_speed_left(0.0f);
            motor_set_speed_right(0.0f);
            sleep_ms(30);

            if (application_yield) break;

            // Backup a fixed distance (blocking)
            drive_straight_distance(BACKUP_DISTANCE_MM, -BACKUP_SPEED);

            if (application_yield) break;

            // Random turn
            uint32_t r = next_rand();
            uint32_t angle_range = (TURN_MAX_DEG - TURN_MIN_DEG + 1);
            uint32_t angle = TURN_MIN_DEG + (r % (angle_range ? angle_range : 1));
            bool turn_left_dir = (r & 1u) == 0;

            if (turn_left_dir) {
                turn_left((float)angle, TURN_SPEED);
            } else {
                turn_right((float)angle, TURN_SPEED);
            }

            // small settle
            motor_set_speed_left(0.0f);
            motor_set_speed_right(0.0f);
            sleep_ms(50);

            continue; // resume forward
        }

        if (application_yield) break;
        sleep_ms(POLL_MS);
    }

    // On exit, ensure motors are stopped
    motor_set_speed_left(0.0f);
    motor_set_speed_right(0.0f);
}
