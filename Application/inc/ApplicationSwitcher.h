#ifndef APPLICATION_SWITCHER_H
#define APPLICATION_SWITCHER_H

#include <stdbool.h>
#include <stdint.h>

extern volatile bool application_yield;

void run_application_switcher();

#endif // APPLICATION_SWITCHER_H
