#ifndef MAZE_BUMP_H
#define MAZE_BUMP_H

#include <stdint.h>

// Run the bump-driven roaming behavior. This function blocks until
// `application_yield` becomes true (set by the application switcher) or
// an external stop occurs.
void run_prison_bot(void);

#endif // MAZE_BUMP_H
