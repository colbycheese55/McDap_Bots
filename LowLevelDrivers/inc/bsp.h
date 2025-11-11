/*
 * Board Support Package for the MSPM0G3507
 */

#ifndef BSP_H_
#define BSP_H_

#include <stdint.h>


// Defines related to clocking system
// Valid clock frequencies are 4 MHz, 32 MHz, 40 MHz, and 80 MHz
#ifdef PROJECT_CLOCK_FREQUENCY
#define CLOCK_FREQUENCY PROJECT_CLOCK_FREQUENCY
#define ONE_MICROSECOND_DELAY PROJECT_ONE_MICROSECOND_DELAY
#else
#define CLOCK_FREQUENCY 32000000 // default clock frequency
#define ONE_MICROSECOND_DELAY 32 // for default clock frequency = 32 MHz
#endif

#define ONE_MILLISECOND_DELAY ((uint32_t)(1000*ONE_MICROSECOND_DELAY))

// Number of clock cycles to wait during power-up
// Note that the project, empty_non_sysconfig uses a value of 16
// #define POWER_STARTUP_DELAY 24 // Valvano, page 74

// Indices for the Pin Control Management (PINCM) Registers
// See Table 6-1 of the MSPM0350x datasheet
#define PA0INDEX    0
#define PA1INDEX    1
#define PA2INDEX    6
#define PA3INDEX    8
#define PA4INDEX    9
#define PA5INDEX    10
#define PA6INDEX    11
#define PA7INDEX    13
#define PA8INDEX    18
#define PA9INDEX    19
#define PA10INDEX   20
#define PA11INDEX   21
#define PA12INDEX   33
#define PA13INDEX   34
#define PA14INDEX   35
#define PA15INDEX   36
#define PA16INDEX   37
#define PA17INDEX   38
#define PA18INDEX   39
#define PA19INDEX   41
#define PA20INDEX   42
#define PA21INDEX   45
#define PA22INDEX   46
#define PA23INDEX   52
#define PA24INDEX   53
#define PA25INDEX   54
#define PA26INDEX   58
#define PA27INDEX   59
#define PA28INDEX   2
#define PA29INDEX   3
#define PA30INDEX   4
#define PA31INDEX   5
#define PB0INDEX    11
#define PB1INDEX    12
#define PB2INDEX    14
#define PB3INDEX    15
#define PB4INDEX    16
#define PB5INDEX    17
#define PB6INDEX    22
#define PB7INDEX    23
#define PB8INDEX    24
#define PB9INDEX    25
#define PB10INDEX   26
#define PB11INDEX   27
#define PB12INDEX   28
#define PB13INDEX   29
#define PB14INDEX   30
#define PB15INDEX   31
#define PB16INDEX   32
#define PB17INDEX   42
#define PB18INDEX   43
#define PB19INDEX   44
#define PB20INDEX   47
#define PB21INDEX   48
#define PB22INDEX   49
#define PB23INDEX   50
#define PB24INDEX   51
#define PB25INDEX   55
#define PB26INDEX   56
#define PB27INDEX   57

// IOMUX modes for GPIO
#define IOMUX_MODE1 ((uint32_t) 0x00000001)
#define IOMUX_MODE2 ((uint32_t) 0x00000002)
#define IOMUX_MODE3 ((uint32_t) 0x00000003)
#define IOMUX_MODE4 ((uint32_t) 0x00000004)
#define IOMUX_MODE5 ((uint32_t) 0x00000005)
#define IOMUX_MODE6 ((uint32_t) 0x00000006)
#define IOMUX_MODE7 ((uint32_t) 0x00000007)
#define IOMUX_MODE8 ((uint32_t) 0x00000008)

// PINCM indices for red LED, RGB LED, and pushbuttons S1 and S2
#define RED_LED_INDEX   PA0INDEX
#define RGB_R_INDEX     PB26INDEX
#define RGB_G_INDEX     PB27INDEX
#define RGB_B_INDEX     PB22INDEX
#define S1_INDEX        PA18INDEX
#define S2_INDEX        PB21INDEX

// LED1 port register and mask
#define RED_LED_PIN 0
#define RED_LED_MASK ((uint32_t) (0x00000001U << RED_LED_PIN))
#define RED_LED_PORT GPIOA

// LED2 port register and masks
#define RGB_R_PIN 26
#define RGB_R_MASK ((uint32_t) (0x00000001U << RGB_R_PIN))
#define RGB_R_PORT GPIOB
#define RGB_G_PIN 27
#define RGB_G_MASK ((uint32_t) (0x00000001U << RGB_G_PIN))
#define RGB_G_PORT GPIOB
#define RGB_B_PIN 22
#define RGB_B_MASK ((uint32_t) (0x00000001U << RGB_B_PIN))
#define RGB_B_PORT GPIOB

#define GPIO_POL_DISABLE    0x00000000
#define GPIO_POL_RISING     0x00000001
#define GPIO_POL_FALLING    0x00000002
#define GPIO_POL_BOTH       0x00000003

// Pushbuttons S1 and S2 port registers and masks
#define S1_PIN 18
#define S1_MASK ((uint32_t) (0x00000001U << S1_PIN))
#define S1_PORT GPIOA
#define S1_POL_RISING ((uint32_t) (GPIO_POL_RISING << (2*(S1_PIN-16))))
#define S1_POL_FALLING ((uint32_t) (GPIO_POL_FALLING << (2*(S1_PIN-16))))
#define S1_INTERRUPT GPIOA_INT_IRQn

#define S2_PIN 21
#define S2_MASK ((uint32_t) (0x00000001U << S2_PIN))
#define S2_PORT GPIOB
#define S2_POL_RISING ((uint32_t) (GPIO_POL_RISING << (2*(S2_PIN-16))))
#define S2_POL_FALLING ((uint32_t) (GPIO_POL_FALLING << (2*(S2_PIN-16))))
#define S2_INTERRUPT GPIOB_INT_IRQn

// Function prototypes
/**
 * @brief  This function is executed in case of error.
 * @retval None
 */
void Error_Handler(void);

/**
 * @brief Function to enable and activate port registers A and B.
 * @param None.
 * @return none
 */
void ActivatePortAandB(void);

/**
 * @brief Configures the LP-MSPM0G3507 launchpad.
 * @param None.
 * @return none
 */
void InitializeLaunchpad(void);

/**
 * @brief Read S1, which is a Normally Open (NO) active-high pushbutton
 * @param none
 * @return 1 if S1 is pressed, 0 otherwise
 */
uint32_t ReadS1(void);

/**
 * @brief Read S2, which is a Normally Open (NO) active-low pushbutton
 * @param none
 * @return 1 if S2 is pressed, 0 otherwise
 */
uint32_t ReadS2(void);

/**
 * @brief Turn on LED1 (red LED)
 * @param none
 * @return none
 */
void TurnOnLED1(void);

/**
 * @brief Turn off LED1 (red LED)
 * @param none
 * @return none
 */
void TurnOffLED1(void);
/**
 * @brief Toggle LED1 (red LED)
 * @param none
 * @return none
 */
void ToggleLED1(void);

/**
 * @brief Turn on LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOnLED2Red(void);

/**
 * @brief Turn off LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOffLED2Red(void);
/**
 * @brief Toggle LED2 (RGB, red)
 * @param none
 * @return none
 */
void ToggleLED2Red(void);

/**
 * @brief Turn on LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOnLED2Green(void);

/**
 * @brief Turn off LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOffLED2Green(void);
/**
 * @brief Toggle LED2 (RGB, red)
 * @param none
 * @return none
 */
void ToggleLED2Green(void);

/**
 * @brief Turn on LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOnLED2Blue(void);

/**
 * @brief Turn off LED2 (RGB, red)
 * @param none
 * @return none
 */
void TurnOffLED2Blue(void);
/**
 * @brief Toggle LED2 (RGB, red)
 * @param none
 * @return none
 */
void ToggleLED2Blue(void);

#endif /* BSP_H_ */
