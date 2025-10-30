#ifndef GPIO_H
#define GPIO_H

#include <stdint.h>

/**
 * @brief Generic GPIO port handle.
 *
 * Each platform defines what GPIO_Port actually is.
 */
#ifdef STM32G071xx
#include "stm32g0xx_hal.h"

typedef struct {
	GPIO_TypeDef* port;
	uint16_t pin;
}GPIO_Handle;

#else //TI MSPM0
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

typedef struct {
	GPIO_Regs* port;
	uint32_t pin;
}GPIO_Handle;

#endif


void gpio_init(GPIO_Handle* gpio, size_t port, uint32_t pin);

/**
 *  @brief      Read a GPIO *INPUT* pin
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to read. One of @ref DL_GPIO_PIN.
 *
 *  @return     The state of the GPIO pin (0 or 1).
 */
uint8_t gpio_read_pin(GPIO_Handle* gpio);

/**
 *  @brief      Update the value of a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio      Pointer to the register overlay for the peripheral
 *  @param[in]  pin       The GPIO pin you want to update. One of @ref DL_GPIO_PIN.
 *  @param[in]  val       The value to set the GPIO pin to (0 or 1).
 */
void gpio_write_pin(GPIO_Handle* gpio, uint8_t val);

/**
 *  @brief      Toggle a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to toggle. One of @ref DL_GPIO_PIN.
 */
void gpio_toggle_pin(GPIO_Handle* gpio);

#endif // GPIO_H
