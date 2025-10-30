#include "../inc/gpio.h"

void gpio_init(GPIO_Handle* gpio, size_t port, uint32_t pin){
    gpio->port = (GPIO_Regs*) port;
    gpio->pin = pin;
}

uint8_t gpio_read_pin(GPIO_Handle* gpio) {
    GPIO_Regs *port = gpio->port;
    return (port->DIN31_0 & gpio->pin) ? 1 : 0;
}

void gpio_write_pin(GPIO_Handle* gpio, uint8_t val) {
    GPIO_Regs *port = gpio->port;
    if (val) {
        port->DOUTSET31_0 = gpio->pin;
    } else {
        port->DOUTCLR31_0 = gpio->pin;
    }
}

void gpio_toggle_pin(GPIO_Handle* gpio) {
    GPIO_Regs *port = gpio->port;
    port->DOUTTGL31_0 = gpio->pin;
}
