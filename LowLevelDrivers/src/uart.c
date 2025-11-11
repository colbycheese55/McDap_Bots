// #include "../inc/uart.h"
// #include <ti/driverlib/dl_uart_main.h>  // ensure prototypes are visible
// #include "ti_msp_dl_config.h"

// static UART_Handle g_uart1 = {0};

// static inline UART_Regs *U1(void) {
//     return (UART_Regs *)UART_1_INST;
// }

// void UART_open(UART_Handle *h) {
//     h->inst = U1();
//     /* SysConfig already did: clock, mux, init, baud, enable.
//        We ensure it's enabled and set a low RX FIFO threshold for snappy reads. */
//     DL_UART_Main_enable(h->inst);
//     DL_UART_Main_setRXFIFOThreshold(h->inst, DL_UART_MAIN_RX_FIFO_LEVEL_ONE_ENTRY);
// }

// void UART_initBLE(void) {
//     /* Bind our singleton to UART1 and prep FIFOs */
//     UART_open(&g_uart1);
//     UART_flushRX(&g_uart1);
//     UART_flushTX(&g_uart1);
// }

// UART_Handle *UART_getBLEHandle(void) {
//     return &g_uart1;
// }

// uint8_t UART_getcBlocking(UART_Handle *h) {
//     while (DL_UART_Main_isRXFIFOEmpty(h->inst)) {
//         /* spin */
//     }
//     return (uint8_t)DL_UART_Main_receiveData(h->inst);
// }

// void UART_putc(UART_Handle *h, uint8_t c) {
//     /* Wait until UART not busy to ensure space, then send a byte */
//     while (DL_UART_Main_isBusy(h->inst)) { /* spin */ }
//     DL_UART_Main_transmitData(h->inst, c);
// }

// void UART_write(UART_Handle *h, const uint8_t *buf, uint32_t len) {
//     for (uint32_t i = 0; i < len; ++i) {
//         UART_putc(h, buf[i]);
//     }
// }

// void UART_flushRX(UART_Handle *h) {
//     while (!DL_UART_Main_isRXFIFOEmpty(h->inst)) {
//         (void)DL_UART_Main_receiveData(h->inst);
//     }
// }

// void UART_flushTX(UART_Handle *h) {
//     /* Wait until the UART finishes transmitting everything */
//     while (DL_UART_Main_isBusy(h->inst)) { /* spin */ }
// }

// /* --------- Compatibility for AP.c --------- */
// uint8_t UART1_InChar(void) {
//     return UART_getcBlocking(&g_uart1);
// }

// void UART1_OutChar(uint8_t c) {
//     UART_putc(&g_uart1, c);
// }

// void UART1_Write(const uint8_t *buf, uint32_t len) {
//     UART_write(&g_uart1, buf, len);
// }