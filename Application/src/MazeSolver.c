
/*
Implement a simple wall-following algorithm based on the right hand rule:
https://andrewyong7338.medium.com/maze-escape-with-wall-following-algorithm-170c35b88e00
The robot will follow the wall on its right side until it exits the maze
*/

#include "../inc/MazeSolver.h"

// setup: have walls about 30mm away from the sides of the robot. Corridoors should be a total of 190 mm wide.

#define UNIT_DISTANCE 10 // unit distance of movement in mm
#define RIGHT_WALL_DETECTED_DIST 70 // distance given by IR sensor under which a right wall is considered to be detected
#define FRONT_WALL_DETECTED_DIST 40 // distance given by IR sensor under which a right wall is considered to be detected
#define ROBOT_LENGTH 180

void run_maze_solver() {
    // if wall on the right and no wall in front, move forward
    if (ir_get_distance(RIGHT) < RIGHT_WALL_DETECTED_DIST && ir_get_distance(CENTER) >= FRONT_WALL_DETECTED_DIST) {
        drive_straight_distance(UNIT_DISTANCE, 0.1);
    }
    // if no wall on the right, turn 90 degrees clockwise then move forward
    else if (ir_get_distance(RIGHT) >= RIGHT_WALL_DETECTED_DIST) {
        // drive straight after detecting no right wall for a bit more so that robot fully clears wall.
        drive_straight_distance(ROBOT_LENGTH, 0.1);
        // after clearing wall, turn right
        turn_right_in_place(120, 0.3);
        drive_straight_distance(UNIT_DISTANCE, 0.1);
    }
    // if wall on the right and wall in front, turn 90 degrees counterclockwise
    else if (ir_get_distance(RIGHT) < RIGHT_WALL_DETECTED_DIST && ir_get_distance(CENTER >= FRONT_WALL_DETECTED_DIST)) {
        turn_left_in_place(120, 0.3);
    }
    return; 
}
