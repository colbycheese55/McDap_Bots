#include "../inc/bumpSwitches.h"


GPIO_Handle farLeftSwitch;
GPIO_Handle midLeftSwitch;
GPIO_Handle midRightSwitch;
GPIO_Handle farRightSwitch;

void bump_switches_init(GPIO_Handle farLeft,
                         GPIO_Handle midLeft,
                         GPIO_Handle midRight,
                         GPIO_Handle farRight) {
    farLeftSwitch = farLeft;
    midLeftSwitch = midLeft;
    midRightSwitch = midRight;
    farRightSwitch = farRight;
}

bool bump_switch_read(Switch sw) {
    switch (sw) {
        case FAR_LEFT:
            return gpio_read_pin(farLeftSwitch) == 0;
        case MID_LEFT:
            return gpio_read_pin(midLeftSwitch) == 0;
        case MID_RIGHT:
            return gpio_read_pin(midRightSwitch) == 0;
        case FAR_RIGHT:
            return gpio_read_pin(farRightSwitch) == 0;
        default:
            return false;
    }
}