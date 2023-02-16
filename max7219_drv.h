#ifndef __MAX7219_DRV_H
#define __MAX7219_DRV_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "main.h"

#define MAX7219_REGISTER_ADDRESS_NOOPERATION   0x00 
#define MAX7219_REGISTER_ADDRESS_DIGIT0        0x01 
#define MAX7219_REGISTER_ADDRESS_DIGIT1        0x02 
#define MAX7219_REGISTER_ADDRESS_DIGIT2        0x03 
#define MAX7219_REGISTER_ADDRESS_DIGIT3        0x04 
#define MAX7219_REGISTER_ADDRESS_DIGIT4        0x05 
#define MAX7219_REGISTER_ADDRESS_DIGIT5        0x06 
#define MAX7219_REGISTER_ADDRESS_DIGIT6        0x07 
#define MAX7219_REGISTER_ADDRESS_DIGIT7        0x08 
#define MAX7219_REGISTER_ADDRESS_DECODEMODE    0x09 
#define MAX7219_REGISTER_ADDRESS_INTENSITY     0x0A 
#define MAX7219_REGISTER_ADDRESS_SCANLLIMIT    0x0B 
#define MAX7219_REGISTER_ADDRESS_SHUTDOWN      0x0C 
#define MAX7219_REGISTER_ADDRESS_DISPLAYTEST   0x0F

#define MAX7219_ROW_SIZE       8
#define MAX7219_COLUMN_SIZE    32
#define MAX7219_BUFFER_SIZE    10


typedef enum {
        SCROLLING_SPEED_NONE        = 0,
        SCROLLING_SPEED_VERY_FAST   = 10,
        SCROLLING_SPEED_FAST        = 20,
        SCROLLING_SPEED_NORMAL      = 40,
        SCROLLING_SPEED_SLOW        = 60,
        SCROLLING_SPPED_VERY_SLOW   = 80,
}SCROLLING_SPEED;


typedef struct max7219_t {
      uint16_t screenBuffer[MAX7219_BUFFER_SIZE][MAX7219_ROW_SIZE+1];
      uint8_t buffer[MAX7219_ROW_SIZE];
}max7219_t;



void max7219_initialize(void);
void max7219_printCharacter(uint8_t character, uint8_t speed);
void max7219_printText(uint8_t* text);
void max7219_scrollText(uint8_t* character, uint8_t speed);

#endif