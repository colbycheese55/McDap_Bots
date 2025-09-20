#ifndef GPIO_H
#define GPIO_H

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>


/**
 *  @brief      Read a GPIO pin (automatically detects input/output mode)
 *
 *  @param[in]  gpio  Pointer to the register overlay for the peripheral
 *  @param[in]  pin   The GPIO pin to read. One of @ref DL_GPIO_PIN.
 *
 *  @return     The state of the GPIO pin (0 or 1).
 *              For input pins: reads the input register (DIN31_0)
 *              For output pins: reads the output register (DOUT31_0)
 */
uint8_t gpio_read_pin(GPIO_Regs* gpio, uint32_t pin) {
    // Check if pin is configured as output by reading DOE (Data Output Enable) register
    uint32_t is_output = gpio->DOE31_0 & pin;
    
    if (is_output) {
        // Pin is configured as output - read from output register
        uint32_t mask = gpio->DOUT31_0 & pin;
        return mask != 0 ? 1 : 0;
    } else {
        // Pin is configured as input - read from input register
        uint32_t mask = gpio->DIN31_0 & pin;
        return mask != 0 ? 1 : 0;
    }
}


/**
 *  @brief      Update the value of a GPIO pin
 *
 *  @param[in]  gpio      Pointer to the register overlay for the peripheral
 *  @param[in]  pin       The GPIO pin you want to update. One of @ref DL_GPIO_PIN.
 *  @param[in]  val       The value to set the GPIO pin to (0 or 1).
 */
void gpio_write_pin(GPIO_Regs* gpio, uint32_t pin, uint8_t val) {
    if (val) {
        gpio->DOUTSET31_0 = pin;
    } else {
        gpio->DOUTCLR31_0 = pin;
    }
}

#endif // GPIO_H