#ifdef __MSPM0G3507__

#include <stdint.h>
#include <string.h>
#include <stdbool.h>

#include "ti_msp_dl_config.h"
#include "HighLevelDrivers/inc/AP_MSPM0.h"
#include "LowLevelDrivers/inc/uart.h"
#include "LowLevelDrivers/inc/dbg_uart0.h"
#include "HighLevelDrivers/inc/motor.h"
#include "LowLevelDrivers/inc/gpio.h"
#include "hardware_init.h"
#include "HighLevelDrivers/inc/SSD1306.h"
#include "HighLevelDrivers/inc/images.h"

int run_bluetooth_controller(void);

#endif // __MSPM0G3507__