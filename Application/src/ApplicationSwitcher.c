#include <stdbool.h>
#include <stdint.h>
#include "../inc/ApplicationSwitcher.h"
#include "../../HighLevelDrivers/inc/motor.h"
#include "../../LowLevelDrivers/inc/sleep.h"

#define INACTIVE        0
#define FOLLOW_LINE     1
#define RC              2
#define MAZE_IR         3
#define MAZE_BUMP       4

volatile bool application_yield = false;
uint8_t state = INACTIVE;


/** APPLICATION API
 *  - each application must be launched by a single function call
 *  - each application function must block so long as `application_yield` is false
 *  - once `application_yield` becomes true, the application function must return ASAP
 *  - therefore, each application function must continually check the value of `application_yield`
 */

void run_application_switcher() {
    while (true) {
        // ensure the motors are stopped
        motor_set_speed_left(0);
        motor_set_speed_right(0);

        application_yield = false;

        // current state logic
        switch (state) {
            case INACTIVE:
                while (!application_yield) {
                    sleep_ms(10);
                }
                break;
            case FOLLOW_LINE:
                // TODO: call line following application
                break;
            case RC:
                // TODO: call bluetooth remote control application
                break;
            case MAZE_IR:
                // TODO: call maze IR application
                break;
            case MAZE_BUMP:
                // TODO: call maze bump application
                break;
        }

        // next state logic
        switch (state) {
            case INACTIVE:
                state = FOLLOW_LINE;
                break;
            case FOLLOW_LINE:
                state = RC;
                break;
            case RC:
                state = MAZE_IR;
                break;
            case MAZE_IR:
                state = MAZE_BUMP;
                break;
            case MAZE_BUMP:
                state = INACTIVE;
                break;
        }
    }
}