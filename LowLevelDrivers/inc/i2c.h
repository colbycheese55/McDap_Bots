#ifndef I2C_H_
#define I2C_H_

#ifdef STM32G071xx
	#include "stm32g0xx_hal.h"
typedef struct {
    I2C_HandleTypeDef *inst;
} I2C_Handle;
#elif __MSPM0G3507__ // TI MSPM0 (note: this macro is generated in Debug/device.opt as a predefined symbol)
	#include <ti/devices/msp/msp.h>
	#include "bsp.h"
typedef struct {
    I2C_Regs *inst;
} I2C_Handle;
#endif

void i2c_init(I2C_Handle *i2cHandle, uint8_t SCLIndex, uint8_t SDAIndex);
uint16_t i2c_send(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count);
void i2c_receive(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count);


#endif /* I2C_H_ */
