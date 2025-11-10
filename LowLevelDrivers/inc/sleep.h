#ifndef SLEEP_H_
#define SLEEP_H_

#include "ti_msp_dl_config.h"


void inline sleep_us(uint32_t us) {
    uint32_t cycles = us * 32;  // assuming 32 MHz clock
    while (cycles--) { __NOP(); }
}

void inline sleep_ms(uint32_t ms) {
    uint32_t cycles = ms * 32000;  // assuming 32 MHz clock
    while (cycles--) { __NOP(); }
}

#endif /* SLEEP_H_ */