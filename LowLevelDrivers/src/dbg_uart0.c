#include "ti_msp_dl_config.h"
#include <ti/driverlib/dl_uart_main.h>
#include <stdint.h>

static inline UART_Regs* U0(void){ return (UART_Regs*)UART_0_INST; }

void DBG_init(void){
    DL_UART_Main_enable(U0()); // SysConfig already configured baud/mux
}

void DBG_putc(char c){
    while (DL_UART_Main_isBusy(U0())) { }
    DL_UART_Main_transmitData(U0(), (uint8_t)c);
}

void DBG_write(const char *s){
    while (*s) DBG_putc(*s++);
}

static const char HEX[]="0123456789ABCDEF";
void DBG_hex(const uint8_t *b, uint32_t n){
    for (uint32_t i=0;i<n;i++){
        uint8_t v=b[i];
        DBG_putc(HEX[(v>>4)&0xF]); DBG_putc(HEX[v&0xF]); DBG_putc(' ');
    }
    DBG_putc('\r'); DBG_putc('\n');
}
