#include "../inc/uart.h"

/* Keep TI includes private to the driver implementation */
#include <ti/devices/msp/msp.h>
#include <ti/driverlib/dl_uart_main.h>

static inline UART_Regs *U(const UART_Handle *h) {
    return (UART_Regs *)h->inst;
}

void UART_boardInit(void)
{
    /* Pins/clocks/baud/pinmux come from SysConfig */
    SYSCFG_DL_init();
}

void UART_open0(UART_Handle *h)
{
    h->inst = (void *)UART_0_INST;

    /* Make sure UART is enabled (SysConfig usually does this already) */
    DL_UART_Main_enable(U(h));

    /* Nice for polling: fire RX check as soon as 1 byte is present */
    DL_UART_Main_setRXFIFOThreshold(U(h), DL_UART_MAIN_RX_FIFO_LEVEL_ONE_ENTRY);

    /* Clear any stale RX */
    UART_flushRX(h);
}

/* ---------- TX ---------- */
void UART_putc(UART_Handle *h, uint8_t c)
{
    DL_UART_Main_transmitDataBlocking(U(h), c);
}

void UART_write(UART_Handle *h, const void *buf, uint32_t len)
{
    const uint8_t *p = (const uint8_t *)buf;
    for (uint32_t i = 0; i < len; ++i) {
        DL_UART_Main_transmitDataBlocking(U(h), p[i]);
    }
}

void UART_puts(UART_Handle *h, const char *s)
{
    while (s && *s) {
        if (*s == '\n') DL_UART_Main_transmitDataBlocking(U(h), '\r'); // CRLF for friendly terminals
        DL_UART_Main_transmitDataBlocking(U(h), (uint8_t)*s++);
    }
}

/* ---------- RX ---------- */
bool UART_tryGetc(UART_Handle *h, uint8_t *out)
{
    return DL_UART_Main_receiveDataCheck(U(h), out);  // returns true if a byte was read into *out
}

uint8_t UART_getcBlocking(UART_Handle *h)
{
    return DL_UART_Main_receiveDataBlocking(U(h));
}

void UART_flushRX(UART_Handle *h)
{
    uint8_t d;
    while (DL_UART_Main_receiveDataCheck(U(h), &d)) { /* discard */ }
}
