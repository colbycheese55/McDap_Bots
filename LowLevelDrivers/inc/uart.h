#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Opaque handle so app code doesn’t include TI headers */
typedef struct {
    void *inst;  /* internal UART instance pointer (UART_Regs*) */
} UART_Handle;

/* Board bring-up (calls SYSCFG_DL_init from your SysConfig) */
void UART_boardInit(void);

/* Open UART0 using your SysConfig instance (PA10/PA11 @ 9600) */
void UART_open0(UART_Handle *h);

/* TX (blocking) */
void UART_putc(UART_Handle *h, uint8_t c);
void UART_write(UART_Handle *h, const void *buf, uint32_t len);
void UART_puts(UART_Handle *h, const char *s);

/* RX */
bool    UART_tryGetc(UART_Handle *h, uint8_t *out);  /* non-blocking: true if a byte read */
uint8_t UART_getcBlocking(UART_Handle *h);           /* blocking read */

/* Utilities */
void UART_flushRX(UART_Handle *h);

#ifdef __cplusplus
}
#endif
#endif /* UART_H */
