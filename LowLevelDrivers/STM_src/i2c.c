#include "i2c.h"
#include "stm32g0xx_hal.h"
#define FALSE 0
#define TRUE 1


/**
 * @brief Sends data to target I2C device. This function is non-blocking.
 *
 * @param hi2c Pointer to I2C interface handler
 * @param target_address I2C device address
 * @param buffer Buffer containing the data to be sent
 * @param count Number of bytes of data to be sent
 */
uint16_t i2c_send(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count) {
	I2C_HandleTypeDef* hi2c = i2cHandle->inst;
	uint16_t device_address = target_address << 1;
	HAL_StatusTypeDef status;

	status = HAL_I2C_Master_Transmit(hi2c, device_address, buffer, count, 1000);
	if (status == HAL_OK) { // successful transmit
		return TRUE;
	}
	else { // transmit failed
		return FALSE;
	}
}

/**
 * @brief Receives data from target I2C device. This function is non-blocking.
 *
 * @param hi2c Pointer to I2C interface handler
 * @param target_address I2C device address
 * @param buffer Buffer storing the data to be received
 * @param count Number of bytes of data to be received
 */
void i2c_receive(I2C_Handle *i2cHandle, uint8_t target_address, uint8_t *buffer, uint16_t count) {
	I2C_HandleTypeDef* hi2c = i2cHandle->inst;
	uint16_t device_address = target_address << 1;
	HAL_I2C_Master_Receive(hi2c, device_address, buffer, count, 1000); // use timeout of 1000 ms
}
