#include "../../LowLevelDrivers/inc/adc.h"

// Returns the measured distance in mm
// Note that measurements less than 80mm are unreliable, and the maximum distance is about 150mm
uint8_t get_IR_distance(ADC12_Regs *adc12);