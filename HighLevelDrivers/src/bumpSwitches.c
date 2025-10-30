// #include "../inc/bumpSwitches.h"


// GPIO_Handle farLeftSwitch;
// GPIO_Handle midLeftSwitch;
// GPIO_Handle midRightSwitch;
// GPIO_Handle farRightSwitch;

// void bump_switches_init(GPIO_Handle farLeft,
//                          GPIO_Handle midLeft,
//                          GPIO_Handle midRight,
//                          GPIO_Handle farRight) {
//     farLeftSwitch = farLeft;
//     midLeftSwitch = midLeft;
//     midRightSwitch = midRight;
//     farRightSwitch = farRight;
// }

// bool bump_switch_read(Switch sw) {
//     switch (sw) {
//         case FAR_LEFT:
//             return gpio_read_pin(farLeftSwitch.gpio, farLeftSwitch.pin) == 1;
//         case MID_LEFT:
//             return gpio_read_pin(midLeftSwitch.gpio, midLeftSwitch.pin) == 1;
//         case MID_RIGHT:
//             return gpio_read_pin(midRightSwitch.gpio, midRightSwitch.pin) == 1;
//         case FAR_RIGHT:
//             return gpio_read_pin(farRightSwitch.gpio, farRightSwitch.pin) == 1;
//         default:
//             return false;
//     }
// }