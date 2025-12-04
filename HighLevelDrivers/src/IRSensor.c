#include "../inc/IRSensor.h"
#include "../../LowLevelDrivers/inc/adc.h"

// Parameters for distance sensor conversion
#define K_IR 100000000000
#define A_IR 2
#define B_IR 55244
#define C_IR 7506171
#define D_IR 0
#define IRmaxl 400

#define FUDGE_FACTOR 1.43 // experimentally determined to match actual distances

ADC_Handle ir_left;
ADC_Handle ir_center;
ADC_Handle ir_right;

void ir_init(ADC_Handle ir_left_handle, ADC_Handle ir_center_handle, ADC_Handle ir_right_handle) {
    ir_left = ir_left_handle;
    ir_center = ir_center_handle;
    ir_right = ir_right_handle;
}


uint32_t ConvertDistanceSensorReading(uint32_t SensorInput)
{
    int64_t ReturnValue = 0;

	// Convert ADC reading to distance (in millimeters) based on
    // transfer function in datasheet.
    ReturnValue = (int64_t) C_IR;
    ReturnValue = (int64_t)(B_IR*SensorInput) + ReturnValue;
    ReturnValue = (int64_t)(A_IR*SensorInput*SensorInput) + ReturnValue;
    ReturnValue = (int64_t)(K_IR/ReturnValue);
    return (uint32_t) ReturnValue;
}

uint8_t ir_get_distance(IR_Sensor_Position position) {
    ADC_Handle *adc;
    switch (position) {
        case LEFT:
            adc = &ir_left;
            break;
        case CENTER:
            adc = &ir_center;
            break;
        case RIGHT:
            adc = &ir_right;
            break;
    }
    
    float percent = adc_get_value(adc);
    uint32_t reading = (uint32_t)(percent * 4095.0f); // convert back to a 12-bit reading
    reading <<= 2; // reading should be 14 bits
    uint32_t distance = ConvertDistanceSensorReading(reading);
    distance /= FUDGE_FACTOR; 

    if (distance > IRmaxl) {
        distance = IRmaxl;
    }

    return (uint8_t) distance;
}


