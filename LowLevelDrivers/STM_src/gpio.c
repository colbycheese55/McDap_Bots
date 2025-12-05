#include "gpio.h"
#include "stm32g0xx_hal.h"


uint8_t gpio_read_pin(GPIO_Handle handle) {
    GPIO_TypeDef *port = (GPIO_TypeDef *)handle.peripheral;
    return (HAL_GPIO_ReadPin(port, handle.pin) == GPIO_PIN_SET);
}

void gpio_write_pin(GPIO_Handle handle, uint8_t val) {
    GPIO_TypeDef *port = (GPIO_TypeDef *)handle.peripheral;
    HAL_GPIO_WritePin(port, handle.pin, val ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void gpio_toggle_pin(GPIO_Handle handle) {
    GPIO_TypeDef *port = (GPIO_TypeDef *)handle.peripheral;
    HAL_GPIO_TogglePin(port, handle.pin);
}

