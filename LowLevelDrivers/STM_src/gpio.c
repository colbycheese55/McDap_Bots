// STM gpio.h implementation
#include "gpio.h"


void gpio_init(GPIO_Handle* gpio, size_t port, uint32_t pin){
	gpio->port = (GPIO_TypeDef *) port;
	gpio-> pin = (uint16_t) pin;
}

uint8_t gpio_read_pin(GPIO_Handle* gpio) {
    return (HAL_GPIO_ReadPin(gpio->port, gpio->pin) == GPIO_PIN_SET);
}

void gpio_write_pin(GPIO_Handle* gpio, uint8_t val) {
    HAL_GPIO_WritePin(gpio->port, gpio->pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_toggle_pin(GPIO_Handle* gpio) {
    HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}
