#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_uart_main.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    UART_Regs *inst;   // bound to UART_1_INST
} UART_Handle;


/* Generic handle-based API (uses the same global UART1 instance) */
void    UART_open(UART_Handle *h);                        // binds to UART1
uint8_t UART_getcBlocking(UART_Handle *h);
void    UART_putc(UART_Handle *h, uint8_t c);
void    UART_write(UART_Handle *h, const uint8_t *buf, uint32_t len);
void    UART_flushRX(UART_Handle *h);
void    UART_flushTX(UART_Handle *h);


#ifdef __cplusplus
}
#endif

#endif /* UART_H */
