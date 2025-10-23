#ifdef TI

#include "../inc/IRSensor.h"
#include "../../LowLevelDrivers/inc/adc.h"

// Parameters for distance sensor conversion
#define K_IR 100000000000
#define A_IR 2
#define B_IR 55244
#define C_IR 7506171
#define D_IR 0
#define IRmaxl 2000

#define FUDGE_FACTOR 5.3 // experimentally determined to match actual distances


uint32_t ConvertDistanceSensorReading(uint32_t SensorInput)
{
    int64_t ReturnValue = 0;

	// Convert ADC reading to distance (in millimeters) based on
    // transfer function in datasheet.
	if (SensorInput < IRmaxl)
    {
        ReturnValue = 800;
    }
    else {
        ReturnValue = (int64_t) C_IR;
        ReturnValue = (int64_t)(B_IR*SensorInput) + ReturnValue;
        ReturnValue = (int64_t)(A_IR*SensorInput*SensorInput) + ReturnValue;
        ReturnValue = (int64_t)(K_IR/ReturnValue);

    }
    return (uint32_t) ReturnValue;
}

uint8_t get_IR_distance(ADC12_Regs *adc12) {
    float percent = adc_get_value(adc12);
    uint32_t reading = (uint32_t)(percent * 4095.0f); // convert back to a 12-bit reading
    uint32_t distance = ConvertDistanceSensorReading(reading);
    distance /= FUDGE_FACTOR; 
    return (uint8_t) distance;
}

#endif //TI

#ifdef STM
#include "adc.h"

#define ADC_VDD 3.3

// Parameters for distance sensor conversion
#define K_IR 100000000000
#define A_IR 2
#define B_IR 55244
#define C_IR 7506171
#define D_IR 0
#define IRmaxl 2000

#define FUDGE_FACTOR 5.3 // experimentally determined to match actual distances

uint32_t ConvertDistanceSensorReading(uint32_t SensorInput)
{
    int64_t ReturnValue = 0;

	// Convert ADC reading to distance (in millimeters) based on
    // transfer function in datasheet.
	if (SensorInput < IRmaxl)
    {
        ReturnValue = 800;
    }
    else {
        ReturnValue = (int64_t) C_IR;
        ReturnValue = (int64_t)(B_IR*SensorInput) + ReturnValue;
        ReturnValue = (int64_t)(A_IR*SensorInput*SensorInput) + ReturnValue;
        ReturnValue = (int64_t)(K_IR/ReturnValue);

    }
    return (uint32_t) ReturnValue;
}

uint8_t get_IR_distance(ADC_HandleTypeDef* hadc1) {

	HAL_ADC_ConfigChannel(hadc1, 0); //hard coded for ADC Input 0 on a given ADC

	HAL_ADC_Start(hadc1);

	HAL_ADC_PollForConversion(hadc1, HAL_MAX_DELAY);
	HAL_ADC_Stop(hadc1);

	uint32_t reading = HAL_ADC_GetValue(hadc1); // convert back to a 12-bit reading
	uint32_t distance = ConvertDistanceSensorReading(reading);
	distance /= FUDGE_FACTOR;
	return (uint8_t) distance;

}

#endif
