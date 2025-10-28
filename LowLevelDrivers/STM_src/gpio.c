// STM gpio.h implementation
#include "gpio.h"
#include "stm32g0xx_hal.h"

uint8_t gpio_read_pin(GPIO_Port *port, uint32_t pin) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    return (HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET);
}

void gpio_write_pin(GPIO_Port *port, uint32_t pin, uint8_t val) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    HAL_GPIO_WritePin(gpio, pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_toggle_pin(GPIO_Port *port, uint32_t pin) {
    GPIO_TypeDef *gpio = (GPIO_TypeDef *)port;
    HAL_GPIO_TogglePin(gpio, pin);
}
