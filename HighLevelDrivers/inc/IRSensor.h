#include "../../LowLevelDrivers/inc/adc.h"


typedef enum {
    LEFT,
    CENTER,
    RIGHT
} IR_Sensor_Position;

void ir_init(ADC_Handle ir_left_handle, ADC_Handle ir_center_handle, ADC_Handle ir_right_handle);

// Returns the measured distance in mm
// Note that measurements less than 80mm are unreliable, and the maximum distance is about 150mm
uint8_t ir_get_distance(IR_Sensor_Position position);




