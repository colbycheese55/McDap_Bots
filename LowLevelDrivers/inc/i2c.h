#ifndef I2C_H_
#define I2C_H_

#include <ti/devices/msp/msp.h>
#include "bsp.h"

void i2c_init(I2C_Regs *i2c, uint8_t SCLIndex, uint8_t SDAIndex);
uint16_t i2c_send(I2C_Regs *i2c, uint8_t target_address, uint8_t *buffer, uint16_t count);
void i2c_receive(I2C_Regs *i2c, uint8_t target_address, uint8_t *buffer, uint16_t count);

#endif /* I2C_H_ */
