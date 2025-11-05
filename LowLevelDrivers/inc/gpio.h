#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

/**
 * @brief Generic GPIO port handle.
 *
 * Each platform defines what GPIO_Port actually is.
 */
typedef struct {
    void* peripheral;  /**< Pointer to the GPIO peripheral registers */
    uint32_t pin;      /**< Pin number within the GPIO port */
} GPIO_Handle;


/**
 *  @brief      Read a GPIO *INPUT* pin
 *
 *  @param[in]  handle   The GPIO handle to read.
 *
 *  @return     The state of the GPIO pin (0 or 1).
 */
uint8_t gpio_read_pin(GPIO_Handle handle);

/**
 *  @brief      Update the value of a GPIO *OUTPUT* pin
 *
 *  @param[in]  handle   The GPIO handle to update.
 *  @param[in]  pin       The GPIO pin you want to update. One of @ref DL_GPIO_PIN.
 *  @param[in]  val       The value to set the GPIO pin to (0 or 1).
 */
void gpio_write_pin(GPIO_Handle handle, uint8_t val);

/**
 *  @brief      Toggle a GPIO *OUTPUT* pin
 *
 *  @param[in]  handle   The GPIO handle to toggle.
 *  @param[in]  pin   The GPIO pin to toggle. One of @ref DL_GPIO_PIN.
 */
void gpio_toggle_pin(GPIO_Handle handle);

#endif // GPIO_H
