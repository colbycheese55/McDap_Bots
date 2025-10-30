#ifndef REFLECTANCE_SENSOR_H
#define REFLECTANCE_SENSOR_H

#include "../../LowLevelDrivers/inc/i2c.h"

/**
 * @file reflectance_sensor.h
 * @brief High-level API for the reflectance sensor array.
 *
 * Provides initialization and read functions for the reflectance sensor
 * connected over I2C. Uses IOMUX pin indices for SCL/SDA configuration.
 */

/**
 * @brief Initialize the reflectance sensor driver.
 *
 * Configures the I2C peripheral pins via the IOMUX and prepares the sensor
 * driver for operation. Call this before calling refsen_read_values().
 *
 * @param[in] i2c       Pointer to the I2C register overlay for the peripheral.
 * @param[in] SCLIndex  IOMUX pin index for the I2C SCL line (index into IOMUX->SECCFG.PINCM[]).
 * @param[in] SDAIndex  IOMUX pin index for the I2C SDA line (index into IOMUX->SECCFG.PINCM[]).
 */
void refsen_init(I2C_Regs *i2c, uint8_t SCLIndex, uint8_t SDAIndex);

/**
 * @brief Read reflectance sensor values.
 *
 * Performs a read of the reflectance sensor and returns the measurement
 * data. The first 15 bits are valid, the 16th bit is always 0.
 * A black surface produces a 1 reading, while a white surface produces a 0 reading.
 *
 * @return uint16_t Packed sensor measurement data.
 */
uint16_t refsen_read_values();

#endif // REFLECTANCE_SENSOR_H