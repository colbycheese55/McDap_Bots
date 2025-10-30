#include "../inc/reflectance_sensor.h"

#define SLAVE_ADDRESS   0x00 // todo

// command bytes
#define INPUT_PORT_0    0x00
#define INPUT_PORT_1    0x01
#define OUTPUT_PORT_0   0x02
#define OUTPUT_PORT_1   0x03
#define CONFIG_PORT_0   0x06
#define CONFIG_PORT_1   0x07

typedef enum {
    INPUT,
    OUTPUT
} GPIO_Direction;

I2C_Regs *i2c;
uint8_t SCLIndex = 0;
uint8_t SDAIndex = 0;

//* ------------- GPIO EXTENDER ------------------- */
void gpio_x_set_direction(GPIO_Direction direction) {
    uint8_t buffer[3];
    buffer[0] = CONFIG_PORT_0;
    if (direction == INPUT) {
        buffer[1] = 0xFE; // all inputs except LSB (LED control)
        buffer[2] = 0xFF;
    } 
    else if (direction == OUTPUT) {
        buffer[1] = 0x00; // all outputs
        buffer[2] = 0x00;
    }
    i2c_send(i2c, SLAVE_ADDRESS, buffer, 3);
}

uint16_t gpio_x_read_inputs() {
    // set command register to the input ports
    uint8_t cmd_buffer[1] = {INPUT_PORT_0};
    i2c_send(i2c, SLAVE_ADDRESS, cmd_buffer, 1);

    // read 2 bytes of input data
    uint8_t res_buffer[2];
    i2c_receive(i2c, SLAVE_ADDRESS, res_buffer, 2);
    uint16_t result = ((uint16_t)res_buffer[1] << 8) | res_buffer[0];
    result >>= 1; // LSB is the LED control
    return result;
}


//* ------------- REFLECTANCE SENSOR ------------------- */
void refsen_init(I2C_Regs *i2c_instance, uint8_t scl_index, uint8_t sda_index) {
    i2c = i2c_instance;
    SCLIndex = scl_index;
    SDAIndex = sda_index;
    i2c_init(i2c, SCLIndex, SDAIndex);

    // set the output ports to always be high (when outputs)
    uint8_t buffer[3] = {OUTPUT_PORT_0, 0xFF, 0xFF};
    i2c_send(i2c, SLAVE_ADDRESS, buffer, 3);
}

uint16_t refsen_read_values() {
    // set the pins to output and hold high (output register is already set to all high)
    gpio_x_set_direction(OUTPUT);

    // wait a small delay then set pins to input
    // TODO: wait
    gpio_x_set_direction(INPUT);

    // wait a small delay and read the input values
    // TODO: wait
    return gpio_x_read_inputs();
}