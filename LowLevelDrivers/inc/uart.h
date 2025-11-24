#ifndef UART_H
#define UART_H

#include <stdint.h>
#include <stdbool.h>


#ifdef __cplusplus
extern "C" {
#endif

#ifdef STM32G071xx //STM32G071xx
#include "stm32g0xx_hal.h"
typedef struct {
    UART_HandleTypeDef *inst;   // bound to UART_1_INST
} UART_Handle;
#elif __MSPM0G3507__ // TI MSPM0
#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_uart_main.h>

typedef struct {
    UART_Regs *inst;   // bound to UART_1_INST
} UART_Handle;
#endif 

/**
 * @brief Initialize/open the BLE UART (UART1) using the SysConfig settings.
 * Must be called once after SYSCFG_DL_init().
 */
void UART_initBLE(void);

/* Generic handle-based API (uses the same global UART1 instance) */
void    UART_open(UART_Handle *h);                        // binds to UART1
uint8_t UART_getcBlocking(UART_Handle *h);
void    UART_putc(UART_Handle *h, uint8_t c);
void    UART_write(UART_Handle *h, const uint8_t *buf, uint32_t len);
void    UART_flushRX(UART_Handle *h);
void    UART_flushTX(UART_Handle *h);

/* Convenience singleton accessor (returns the internal UART1 handle) */
UART_Handle *UART_getBLEHandle(void);

/* --- Compatibility symbols expected by AP.c --- */
uint8_t UART1_InChar(void);
void    UART1_OutChar(uint8_t c);
void    UART1_Write(const uint8_t *buf, uint32_t len);

#ifdef __cplusplus
}
#endif

#endif /* UART_H */
