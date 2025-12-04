#include <stdint.h>
#include "../inc/LineFollow.h"
#include "../inc/ApplicationSwitcher.h"
#include "../../HighLevelDrivers/inc/reflectance_sensor.h"
#include "../../HighLevelDrivers/inc/motor.h"

#define MIDPONT     7
#define P           0.10 // TODO
#define D           0.0 // TODO
#define BASE_SPEED  0.2f

int8_t prev_error = 0;


uint8_t get_left_offset(uint16_t reading) {
    uint8_t count = 0;
    while ((reading & 0x4000) == 0 && count < 15) {
        reading <<= 1;
        count++;
    }
    return count;
}

uint8_t get_right_offset(uint16_t reading) {
    uint8_t count = 0;
    while ((reading & 0x0001) == 0 && count < 15) {
        reading >>= 1;
        count++;
    }
    return count;
}


void line_follow_run() {
    while (!application_yield) {
        // read the sensor, compute error and correction
        uint16_t sensor_reading = refsen_read_values();

        uint8_t left_offset = get_left_offset(sensor_reading);
        uint8_t right_offset = get_right_offset(sensor_reading);

        int8_t error = (MIDPONT - left_offset) - (MIDPONT - right_offset); // positive is right error, negative is left error

        int8_t derivative = error - prev_error;
        prev_error = error;

        float correction = P * error + D * derivative;


        // dynamic velocity scaling based on error
        float translational_speed = BASE_SPEED;
        float angular_speed = correction * BASE_SPEED;
        // if (error > 1) {
        //     translational_speed /= error;
        //     angular_speed /= error;
        // }

        drive_arc(translational_speed, angular_speed);
    }
}