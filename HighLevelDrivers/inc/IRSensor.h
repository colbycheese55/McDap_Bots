#ifndef IRSENSOR_H_
#define IRSENSOR_H_

#include "../../LowLevelDrivers/inc/adc.h"


typedef enum {
    LEFT,
    CENTER,
    RIGHT
} IR_Sensor_Position;

void ir_init(ADC_Handle ir_left_handle, ADC_Handle ir_center_handle, ADC_Handle ir_right_handle);

/* Returns the measured distance in mm
*  Range is 0mm to 400mm from the front of the robot.
*  Note: If no object is in range, the sensor oscillates between 200mm and 400mm.
*/
uint8_t ir_get_distance(IR_Sensor_Position position);


#endif /* IRSENSOR_H_ */
