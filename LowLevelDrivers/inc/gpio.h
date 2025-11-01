#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
 * @brief Generic GPIO port handle.
 *
 * Each platform defines what GPIO_Port actually is.
 */
typedef struct GPIO_Port GPIO_Port;


/**
 *  @brief      Read a GPIO *INPUT* pin
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to read. One of @ref DL_GPIO_PIN.
 *
 *  @return     The state of the GPIO pin (0 or 1).
 */
uint8_t gpio_read_pin(GPIO_Port* gpio, uint32_t pin);

/**
 *  @brief      Update the value of a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio      Pointer to the register overlay for the peripheral
 *  @param[in]  pin       The GPIO pin you want to update. One of @ref DL_GPIO_PIN.
 *  @param[in]  val       The value to set the GPIO pin to (0 or 1).
 */
void gpio_write_pin(GPIO_Port* gpio, uint32_t pin, uint8_t val);

/**
 *  @brief      Toggle a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to toggle. One of @ref DL_GPIO_PIN.
 */
void gpio_toggle_pin(GPIO_Port* gpio, uint32_t pin);

#endif // GPIO_H
