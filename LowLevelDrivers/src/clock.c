#include <stdint.h>

void ClockDelay(uint32_t cycles)
{
    for (volatile uint32_t i = 0; i < cycles; i++);
}

void ClockDelay_1ms(uint32_t ms)
{
    for (uint32_t i = 0; i < ms; i++) {
        ClockDelay(80000);  // Approx for 80MHz
    }
}