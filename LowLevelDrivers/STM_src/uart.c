#include "../inc/uart.h"
#include "stm32g0xx_hal.h"


static UART_Handle g_uart1 = {0}; //Bluetooth UART

void UART_open(UART_Handle *h) {

    // Ensure UART is initialized (CubeMX normally handles this)
    if (h->inst->gState == HAL_UART_STATE_RESET) {
        // Optional: user could reinit here if needed
        HAL_UART_Init(h->inst);
    }

    __HAL_UART_ENABLE(h->inst);
}

void UART_initBLE(void) {
    UART_open(&g_uart1);
    UART_flushRX(&g_uart1);
    UART_flushTX(&g_uart1);
}

UART_Handle *UART_getBLEHandle(void) {
    return &g_uart1;
}

uint8_t UART_getcBlocking(UART_Handle *h) {
    uint8_t c;
    HAL_UART_Receive(h->inst, &c, 1, HAL_MAX_DELAY); // blocking receive
    return c;
}

void UART_putc(UART_Handle *h, uint8_t c) {
    HAL_UART_Transmit(h->inst, &c, 1, HAL_MAX_DELAY);
}

void UART_write(UART_Handle *h, const uint8_t *buf, uint32_t len) {
    HAL_UART_Transmit(h->inst, (uint8_t *)buf, len, HAL_MAX_DELAY);
}

void UART_flushRX(UART_Handle *h) {
    uint8_t tmp;
    while (__HAL_UART_GET_FLAG(h->inst, UART_FLAG_RXNE)) {
        HAL_UART_Receive(h->inst, &tmp, 1, 10);
    }
}

void UART_flushTX(UART_Handle *h) {
    // Wait for transmission to complete
    while (__HAL_UART_GET_FLAG(h->inst, UART_FLAG_BUSY)) {
        // spin
    }
}

/* --------- Compatibility for AP.c --------- */
uint8_t UART1_InChar(void) {
    return UART_getcBlocking(&g_uart1);
}

void UART1_OutChar(uint8_t c) {
    UART_putc(&g_uart1, c);
}

void UART1_Write(const uint8_t *buf, uint32_t len) {
    UART_write(&g_uart1, buf, len);
}
