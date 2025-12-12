#include "HighLevelDrivers/inc/SSD1306.h"
// #include "HighLevelDrivers/inc/images.h"
#include "LowLevelDrivers/inc/i2c.h"

void OLED_Example_App() {
    SSD1306_ClearBuffer(); // Clear the data buffer for the OLED controller

    // SSD1306_DrawBMP(0,64,delong,7,SSD1306_WHITE);
    // SSD1306_DrawBMP(32, 64, down_arrow, 7, SSD1306_WHITE);
    SSD1306_OutBuffer();  // Output to the OLED

    SSD1306_SetCursor(0,0);
    SSD1306_OutString("I'm Peter");
}