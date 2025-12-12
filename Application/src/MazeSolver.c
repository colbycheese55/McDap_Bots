
/*
Implement a simple wall-following algorithm based on the right hand rule:
https://andrewyong7338.medium.com/maze-escape-with-wall-following-algorithm-170c35b88e00
The robot will follow the wall on its right side until it exits the maze
*/

#include "../inc/MazeSolver.h"
#include "../inc/ApplicationSwitcher.h"

// setup: have walls about 30mm away from the sides of the robot. Corridoors should be a total of 190 mm wide.

#define UNIT_DISTANCE 5 // unit distance of movement in mm
#define RIGHT_WALL_DETECTED_DIST 120 // distance given by IR sensor under which a right wall is considered to be detected
#define FRONT_WALL_DETECTED_DIST 80 // distance given by IR sensor under which a right wall is considered to be detected
#define ROBOT_LENGTH 270

void run_maze_solver() {

    while (!application_yield) {
        uint32_t right_distance = ir_get_distance(RIGHT);
        uint32_t front_distance = ir_get_distance(CENTER);
        bool anyBump = false;
        // if wall on the right and no wall in front, move forward
        if (right_distance < RIGHT_WALL_DETECTED_DIST && front_distance >= FRONT_WALL_DETECTED_DIST) {
            anyBump = drive_straight_distance_until_bump(UNIT_DISTANCE, 0.3);
        }
        // if no wall on the right, turn 90 degrees clockwise then move forward
        else if (right_distance >= RIGHT_WALL_DETECTED_DIST) {
            // drive straight after detecting no right wall for a bit more so that robot fully clears wall.
            anyBump = drive_straight_distance_until_bump(ROBOT_LENGTH, 0.3);
            // after clearing wall, turn right
            turn_right_in_place(110, 0.3);
            anyBump = drive_straight_distance_until_bump(ROBOT_LENGTH, 0.3);
        }
        // if wall on the right and wall in front, turn 90 degrees counterclockwise
        else if (right_distance < RIGHT_WALL_DETECTED_DIST && front_distance < FRONT_WALL_DETECTED_DIST) {
            turn_left_in_place(150, 0.3);
        }

        // if bumped into wall, back off a bit
        if (anyBump == true) {
            if (bump_switch_read(FAR_RIGHT) == 1) {
                drive_straight_distance(ROBOT_LENGTH/2, -0.1);
                turn_left_in_place(45,0.3); 
            }
            else {
                drive_straight_distance(ROBOT_LENGTH/2, -0.1);
            }
        }
        anyBump = false;
    }

    return; 
}
