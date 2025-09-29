#ifndef __CLOCK_H__
#define __CLOCK_H__

#include <ti/devices/msp/msp.h>
#include "bsp.h"

/**
 * Configure the MSPM0 clock to run at 4MHz or 32MHz
 * @param freq is either 32 MHZ or 4 MHZ
 * @return none
 * @note  Since the crystal is not used, the bus clock will not be  accurate
 * @see Clock_Freq()
 * @brief  Initialize clock to 4 or 32 MHz
 */
void ClockInitialization_4MHz_32MHz(uint32_t freq);

/**
 * Configure the MSPM0 clock to run at 80 MHz
 * @param enablePA14 1 means clock output on PA14, 0 means no output on PA14
 * @return none
 * @note  Since the crystal is used, the bus clock will be very accurate
 * @see Clock_Freq()
 * @brief  Initialize clock to 80 MHz
 */
void ClockInitialization_80MHz(uint32_t enablePA14);

/**
 * Configure the MSPM0 clock to run at 40 MHz
 * @param none
 * @return none 
 * @note  Since the crystal is used, the bus clock will be very accurate
 * @see Clock_Freq()
 * @brief  Initialize clock to 40 MHz
 */
void ClockInitialization_40MHz(void);

/** Valvano's version of this function
 * Simple delay function which delays about cycles bus cycles.
 * It is implemented with a for-loop and is approximate.
 * @param  cycles is the number of bus cycles to wait
 * @return none 
 * @note For a more accurate time delay, you could use the SysTick module.
 * @brief  Software implementation of a busy-wait delay
 */
//void Clock_Delay(uint32_t cycles) __attribute__((noinline));

/**
 * @brief Create time delay based on number of clock cycles.
 * @param cycles: Note that cycles must be greater than 1.
 * @return none
 */
void ClockDelay(uint32_t cycles);

/**
 * Simple delay function which delays about ms milliseconds.
 * It is implemented with a nested for-loop and is approximate.
 * @param  ms is the number of milliseconds to wait
 * @return none 
 * @note For a more accurate time delay, you could use the SysTick module.
 * @brief  Software implementation of a busy-wait delay
 */
void ClockDelay_1ms(uint32_t ms);

/**
 * Initialize the 24-bit SysTick timer.
 * @param  none
 * @return none
 * @note Call this before using SysTick_Wait or SysTick_Wait10ms.
 * @brief  SysTick for busy-wait delay
 */
void SysTick_Init(void);

/**
 * Delay function which delays about delay bus cycles.
 * It is implemented with SysTick and is accurate.
 * @param  delay is the number of bus cycles to wait
 * @return none 
 * @brief  SysTick implementation of a busy-wait delay
 */
void SysTick_Wait(uint32_t delay);

/**
 * Delay function which delays about t*10ms time,
 * assuming the bus clock is 80 MHz.
 * It is implemented with SysTick and is accurate.
 * @param  t is the time to wait (units 10ms)
 * @return none
 * @brief  SysTick implementation of a busy-wait delay
 */
void SysTick_Wait10ms(uint32_t t);

#endif
