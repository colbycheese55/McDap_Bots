#ifndef SLEEP_H_
#define SLEEP_H_

#ifdef STM32G071xx

void static inline sleep_ms(uint32_t ms) {
    HAL_Delay(ms);
}

void static inline sleep_us(uint32_t us) {
    uint32_t cycles = us * 2;
    while (cycles--);
}

#elif __MSPM0G3507__
#include "ti_msp_dl_config.h"

void inline sleep_us(uint32_t us) {
    uint32_t cycles = us * 32;  // assuming 32 MHz clock
    while (cycles--) { __NOP(); }
}

void inline sleep_ms(uint32_t ms) {
    uint32_t cycles = ms * 32000;  // assuming 32 MHz clock
    while (cycles--) { __NOP(); }
}

#endif // __MSPM0G3507__
#endif /* SLEEP_H_ */
