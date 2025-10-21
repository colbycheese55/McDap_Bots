#ifndef GPIO_H
#define GPIO_H

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>


/**
 *  @brief      Read a GPIO *INPUT* pin)
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to read. One of @ref DL_GPIO_PIN.
 *
 *  @return     The state of the GPIO pin (0 or 1).
 */
static inline uint8_t gpio_read_pin(GPIO_Regs* gpio, uint32_t pin) {
    return (gpio->DIN31_0 & pin) ? 1 : 0;
}

/**
 *  @brief      Update the value of a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio      Pointer to the register overlay for the peripheral
 *  @param[in]  pin       The GPIO pin you want to update. One of @ref DL_GPIO_PIN.
 *  @param[in]  val       The value to set the GPIO pin to (0 or 1).
 */
static inline void gpio_write_pin(GPIO_Regs* gpio, uint32_t pin, uint8_t val) {
    if (val) {
        gpio->DOUTSET31_0 = pin;
    } else {
        gpio->DOUTCLR31_0 = pin;
    }
}

/**
 *  @brief      Toggle a GPIO *OUTPUT* pin
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to toggle. One of @ref DL_GPIO_PIN.
 */
static inline void gpio_toggle_pin(GPIO_Regs* gpio, uint32_t pin) {
    gpio->DOUTTGL31_0 = pin;
}

#endif // GPIO_H