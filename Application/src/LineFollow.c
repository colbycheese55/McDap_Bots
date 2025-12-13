#include <stdint.h>
#include <stdlib.h>
#include <math.h>
#include "../inc/LineFollow.h"
#include "../inc/ApplicationSwitcher.h"
#include "../../HighLevelDrivers/inc/reflectance_sensor.h"
#include "../../HighLevelDrivers/inc/motor.h"

#define MIDPONT     7
#define P           0.10 // TODO
#define D           0.05 // TODO
#define BASE_SPEED  0.3f
#define NUM_SAMPLES 5

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

int compare_ints(const void *a, const void *b) {
    return (*(uint8_t*)a - *(uint8_t*)b);
}

uint8_t get_median(uint8_t* array, uint8_t length) {
    qsort(array, length, sizeof(uint8_t), compare_ints);
    return array[length / 2];
}


void line_follow_run() {
    while (!application_yield) {
        // read the sensor, compute error and correction
        uint8_t left_offsets[NUM_SAMPLES];
        uint8_t right_offsets[NUM_SAMPLES];
        for (uint8_t i = 0; i < NUM_SAMPLES; i++) {
            uint16_t sensor_reading = refsen_read_values();

            left_offsets[i] = get_left_offset(sensor_reading);
            right_offsets[i] = get_right_offset(sensor_reading);
        }

        uint8_t left_offset = get_median(left_offsets, NUM_SAMPLES);
        uint8_t right_offset = get_median(right_offsets, NUM_SAMPLES);

        int8_t error = (MIDPONT - left_offset) - (MIDPONT - right_offset); // positive is right error, negative is left error

        int8_t derivative = error - prev_error;
        prev_error = error;

        float correction = P * error + D * derivative;


        // dynamic velocity scaling based on error
        float translational_speed = BASE_SPEED;
        float angular_speed = correction * BASE_SPEED;
        if (error > 1) {
            translational_speed /= pow(error, 0.33);
            angular_speed /= pow(error, 0.33);
        }

        drive_arc(translational_speed, angular_speed);
    }
}