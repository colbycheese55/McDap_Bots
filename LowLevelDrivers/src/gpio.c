#include "gpio.h"
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_common.h>

uint8_t gpio_read_pin(GPIO_Regs* gpio, uint32_t pin) {
    return (gpio->DIN31_0 & pin) ? 1 : 0;
}

void gpio_write_pin(GPIO_Regs* gpio, uint32_t pin, uint8_t val) {
    if (val) {
        gpio->DOUTSET31_0 = pin;
    } else {
        gpio->DOUTCLR31_0 = pin;
    }
}

void gpio_toggle_pin(GPIO_Regs* gpio, uint32_t pin) {
    gpio->DOUTTGL31_0 = pin;
}
