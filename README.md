# MAX7219_DotMatrix
This is library supports the MAX7219 dot matrix display, The entire code was written from scratch within two days for a product but the concept of the design changed to NeoPixel RGB Matrix. 

So instead of making this code a waste, I have decided to make it opensource so that others can use it and upgrade to fit their project.

The code was written to support STM32 series using HAL Driver library.

## Example Code
```
#include "main.h"
#include "max7219_drv.h"

void int (void) {
   
   max7219_initialize();
   while(1) {
        max7219_scrollText((uint8_t*)"Pulsetronics    ", SCROLLING_SPEED_FAST);
        max7219_scrollText((uint8_t*)"Written by Ayinde Olayiwola  ", SCROLLING_SPEED_VERY_FAST);
        max7219_printText("Olay ");
        HAL_Delay(500);
        max7219_printCharacter('I', 1);
        HAL_Delay(500);
   }
 }
```
