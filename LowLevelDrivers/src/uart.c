#include "../inc/uart.h"
#include <ti/driverlib/dl_uart_main.h>  // ensure prototypes are visible
#include "ti_msp_dl_config.h"


void UART_open(UART_Handle *h) {
    /* SysConfig already did: clock, mux, init, baud, enable.
       We ensure it's enabled and set a low RX FIFO threshold for snappy reads. */
    DL_UART_Main_enable(h->inst);
    DL_UART_Main_setRXFIFOThreshold(h->inst, DL_UART_MAIN_RX_FIFO_LEVEL_ONE_ENTRY);
}

uint8_t UART_getcBlocking(UART_Handle *h) {
    while (DL_UART_Main_isRXFIFOEmpty(h->inst)) {
        /* spin */
    }
    return (uint8_t)DL_UART_Main_receiveData(h->inst);
}

void UART_putc(UART_Handle *h, uint8_t c) {
    /* Wait until UART not busy to ensure space, then send a byte */
    while (DL_UART_Main_isBusy(h->inst)) { /* spin */ }
    DL_UART_Main_transmitData(h->inst, c);
}

void UART_write(UART_Handle *h, const uint8_t *buf, uint32_t len) {
    for (uint32_t i = 0; i < len; ++i) {
        UART_putc(h, buf[i]);
    }
}

void UART_flushRX(UART_Handle *h) {
    while (!DL_UART_Main_isRXFIFOEmpty(h->inst)) {
        (void)DL_UART_Main_receiveData(h->inst);
    }
}

void UART_flushTX(UART_Handle *h) {
    /* Wait until the UART finishes transmitting everything */
    while (DL_UART_Main_isBusy(h->inst)) { /* spin */ }
}
