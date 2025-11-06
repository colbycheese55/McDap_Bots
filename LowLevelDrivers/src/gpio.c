#include "../inc/gpio.h"

void gpio_init(GPIO_Handle* gpio, size_t port, uint32_t pin){
    gpio->port = (GPIO_Regs*) port;
    gpio->pin = pin;
}

uint8_t gpio_read_pin(GPIO_Handle handle) {
    GPIO_Regs *gpio = (GPIO_Regs *)handle.peripheral;
    return (gpio->DIN31_0 & handle.pin) ? 1 : 0;
}

void gpio_write_pin(GPIO_Handle handle, uint8_t val) {
    GPIO_Regs *gpio = (GPIO_Regs *)handle.peripheral;
    if (val) {
        gpio->DOUTSET31_0 = handle.pin;
    } else {
        gpio->DOUTCLR31_0 = handle.pin;
    }
}

void gpio_toggle_pin(GPIO_Handle handle) {
    GPIO_Regs *gpio = (GPIO_Regs *)handle.peripheral;
    gpio->DOUTTGL31_0 = handle.pin;
}
