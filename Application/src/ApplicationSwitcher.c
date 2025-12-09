#include <stdbool.h>
#include <stdint.h>
#include "../inc/ApplicationSwitcher.h"
#include "../inc/MazeSolver.h"
#include "../../HighLevelDrivers/inc/motor.h"
#include "../../LowLevelDrivers/inc/sleep.h"
#include "../inc/maze_bump.h"

#include "../inc/LineFollow.h"

#define INACTIVE        0
#define FOLLOW_LINE     1
#define RC              2
#define MAZE_IR         3
#define MAZE_BUMP       4

volatile bool application_yield = false;
uint8_t state = MAZE_IR;


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
        sleep_ms(1000);

        // current state logic
        switch (state) {
            case INACTIVE:
                while (!application_yield) {
                    sleep_ms(10);
                }
                break;
            case FOLLOW_LINE:
                line_follow_run();
                break;
            case RC:
                // TODO: call bluetooth remote control application
                break;
            case MAZE_IR:
                while (!application_yield) {
                    run_maze_solver();
                    sleep_ms(10);
                }
                break;
            case MAZE_BUMP:
                run_maze_bump();
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