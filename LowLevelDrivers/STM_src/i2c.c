#include "i2c.h"


/**
 * @brief Sends data to target I2C device. This function is non-blocking.
 *
 * @param hi2c Pointer to I2C interface handler
 * @param target_address I2C device address
 * @param buffer Buffer containing the data to be sent
 * @param count Number of bytes of data to be sent
 */
void i2c_send(I2C_HandleTypeDef *hi2c, uint8_t target_address, uint8_t *buffer, uint16_t count) {
	uint16_t device_address = target_address << 1;
	HAL_I2C_Master_Transmit(hi2c, device_address, buffer, count, 1000); // use timeout of 1000 ms
}

/**
 * @brief Receives data from target I2C device. This function is non-blocking.
 *
 * @param hi2c Pointer to I2C interface handler
 * @param target_address I2C device address
 * @param buffer Buffer storing the data to be received
 * @param count Number of bytes of data to be received
 */
void i2c_receive(I2C_HandleTypeDef* hi2c, uint8_t target_address, uint8_t *buffer, uint16_t count) {
	uint16_t device_address = target_address << 1;
	HAL_I2C_Master_Receive(hi2c, device_address, buffer, count, 1000); // use timeout of 1000 ms
}
