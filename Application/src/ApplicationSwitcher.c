#include <stdbool.h>
#include <stdint.h>
#include "../inc/ApplicationSwitcher.h"

#include "../../HighLevelDrivers/inc/motor.h"
#include "../../LowLevelDrivers/inc/sleep.h"
#include "../../HighLevelDrivers/inc/SSD1306.h"

#include "../inc/LineFollow.h"
#include "../inc/MazeSolver.h"
#include "../inc/BluetoothController.h"
#include "../inc/PrisonBot.h"

#define INACTIVE        0
#define FOLLOW_LINE     1
#define RC              2
#define MAZE_IR         3
#define PRISON_BOT      4

volatile bool application_yield = false;
uint8_t state = INACTIVE;


void write_label(char* label) {
    SSD1306_ClearBuffer();
    SSD1306_OutBuffer();
    SSD1306_OutString(label);
}

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
                write_label("Inactive");
                while (!application_yield) {
                    sleep_ms(10);
                }
                break;
            case FOLLOW_LINE:
                if (LINE_FOLLOW_DISABLE == 0) {
                    write_label("Running Line Follower");
                    line_follow_run();
                }
                break;
            case RC:
                #ifdef __MSPM0G3507__
                write_label("Running RC Controller");
                run_bluetooth_controller();
                #endif
                break;
            case MAZE_IR:
                write_label("Running Maze Solver");
                run_maze_solver();
                break;
            case PRISON_BOT:
                write_label("Running Prison Bot");
                run_prison_bot();
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
                state = PRISON_BOT;
                break;
            case PRISON_BOT:
                state = INACTIVE;
                break;
        }
    }
}