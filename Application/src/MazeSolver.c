
/*
Implement a simple wall-following algorithm based on the right hand rule:
https://andrewyong7338.medium.com/maze-escape-with-wall-following-algorithm-170c35b88e00
The robot will follow the wall on its right side until it exits the maze
*/

#include "../inc/MazeSolver.h"

#define UNIT_DISTANCE 100 // unit distance of movement in mm
#define WALL_DETECTED_DIST 150 // distance given by IR sensor under which a wall is considered to be detected

void run_maze_solver() {
    // if wall on the right and no wall in front, move forward
    if (ir_get_distance(RIGHT) < WALL_DETECTED_DIST && ir_get_distance(CENTER) >= WALL_DETECTED_DIST) {
        drive_straight_distance(UNIT_DISTANCE, 0.5);
    }
    // if no wall on the right, turn 90 degrees clockwise then move forward
    else if (ir_get_distance(RIGHT) >= WALL_DETECTED_DIST) {
        turn_right_in_place(90, 0.1);
        drive_straight_distance(UNIT_DISTANCE, 0.5);
    }
    // if wall on the right and wall in front, turn 90 degrees counterclockwise
    else if (ir_get_distance(RIGHT) < WALL_DETECTED_DIST && ir_get_distance(CENTER >= WALL_DETECTED_DIST)) {
        turn_left_in_place(90, 0.1);
    }
    return; 
}
