#include "max7219_drv.h"
#include "font_5x7_drv.h"

max7219_t max7219 = {
          .screenBuffer = {{0},{0},{0},{0},{0},{0},{0},{0},{0}},
};


static void __max7219_dataShiftOut(uint8_t data){
     for (uint8_t shifter=0; shifter<MAX7219_ROW_SIZE; shifter++) {
          HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, RESET);
          HAL_GPIO_WritePin(DIN_GPIO_Port, DIN_Pin, (data&0x80));
          HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, SET);
          data <<= 1;
     }
}

static void __max7219_CS_low(void) {
     HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, RESET);
}

static void __max7219_CS_high(void) {
     HAL_GPIO_WritePin(CS_GPIO_Port, CS_Pin, SET);
}

static void __max7219_writeSerialData (uint8_t address, uint8_t command) {
     __max7219_dataShiftOut(address);
     __max7219_dataShiftOut(command);
}

static void __max7219_writePixel(void) {  
     for(uint8_t row=0; row<8; row++) {
          __max7219_CS_low();
         for(int count=1; count<((MAX7219_COLUMN_SIZE/MAX7219_ROW_SIZE)+1); count++) {
              __max7219_writeSerialData(row+1,  max7219.screenBuffer[((MAX7219_COLUMN_SIZE/MAX7219_ROW_SIZE)+1)-count][row]); 
         }
          __max7219_CS_high();
     }
}

static void __max7219_clearScreenBuffer(void) {
     for(int buffer_count=0; buffer_count<MAX7219_BUFFER_SIZE; buffer_count++) {
          for(int row=0; row<(MAX7219_ROW_SIZE+1); row++)
              max7219.screenBuffer[buffer_count][row] = 0;
     }
}
static void __max7219_clearDisplay(void) {

     HAL_GPIO_WritePin(CLK_GPIO_Port, CLK_Pin, RESET);
     HAL_GPIO_WritePin(DIN_GPIO_Port, DIN_Pin, RESET);
     HAL_GPIO_WritePin(CS_GPIO_Port,  CS_Pin,  RESET);
     __max7219_clearScreenBuffer();
     __max7219_writePixel();
} 

static void __max7219_registerAddressCommands(uint8_t address, uint8_t command) {
        for(int buffer_count =0; buffer_count< (MAX7219_COLUMN_SIZE/MAX7219_ROW_SIZE); buffer_count++)
            __max7219_writeSerialData(address, command); 
}

static void __max7219_registerAddressScanLimit(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_SCANLLIMIT,  0x07); 
     __max7219_CS_high();
}

static void __max7219_registerAddressDecodeMode(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_DECODEMODE,  0x00);   
     __max7219_CS_high(); 
}

static void __max7219_registerAddressShutDown(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_SHUTDOWN,    0x00);
     __max7219_CS_high();
}

static void __max7219_registerAddressTurnOn(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_SHUTDOWN,    0x01);   
     __max7219_CS_high();
} 

static void __max7219_registerAddressDisplayTest(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_DISPLAYTEST, 0x00);
     __max7219_CS_high();
}

static void __max7219_registerAddressIntensity(void) {
     __max7219_CS_low();
     __max7219_registerAddressCommands(MAX7219_REGISTER_ADDRESS_INTENSITY,   0x07);
     __max7219_CS_high();
}

void max7219_initialize(void) {    
     __max7219_registerAddressScanLimit();
     __max7219_registerAddressDecodeMode();
     __max7219_registerAddressTurnOn();
     __max7219_registerAddressDisplayTest();
     __max7219_registerAddressIntensity();
     __max7219_clearDisplay(); 
}

static void __max7219_swapBuffer(uint8_t* buffer) {
     for(uint8_t bufferCount=0; bufferCount<MAX7219_ROW_SIZE; bufferCount++) {
         buffer[bufferCount]  = max7219.buffer[bufferCount] & 0xF0;
         buffer[bufferCount] |= max7219.buffer[bufferCount] & 0x0F;
     }
}

static void __max7219_normalRotation(uint8_t* buffer) {
     uint8_t mask = 0x80;

     for(uint8_t location=0; location<(FONT_ROW+1); location++) {
        for(uint8_t loc=0; loc<FONT_COLUMN; loc++) {
           max7219.buffer[location] |=  ((buffer[loc] & mask) << location);
           max7219.buffer[location] >>=1;
        }
        mask >>=1; 
     }
     __max7219_swapBuffer(buffer);
}

static void __max7219_getPrintableBuffer(uint8_t character, uint8_t* arrBuffer) {
    memset(max7219.buffer, 0, 8);
    for(uint8_t col=0; col<5; col++)
        arrBuffer[col] = font_5x7[character-0x10][col];
     __max7219_normalRotation(arrBuffer);     
}

static void __max7219_loadCharBufferIntoScreenBuffer(uint8_t loc, uint8_t* buffer) {
     for(uint8_t row=0; row<MAX7219_ROW_SIZE; row++)
         max7219.screenBuffer[loc][row] = buffer[row]; 
}

static void __max7219_leftShiftScreenBuffer(void) {
     for(int buffer_cnt=4; buffer_cnt>0; buffer_cnt--) {      
         for(uint8_t column = 0; column < 8; column++)
             max7219.screenBuffer[buffer_cnt][column] >>= 1;
     }
}

static void __max7219_loadInitialBufferToFirstScreenBuffer(void) {
     for(uint8_t column = 0; column < 8; column++)
         max7219.screenBuffer[4][column] |= ((max7219.screenBuffer[0][column] & 0x01) << 8);
}

static void __max7219_loadScreenBuffers(void) {
     for(int buffer_cnt=4; buffer_cnt>1; buffer_cnt--) {
         for(uint8_t column = 0; column < 8; column++)
             max7219.screenBuffer[buffer_cnt-1][column] |= ((max7219.screenBuffer[buffer_cnt][column] & 0x01) << 8);
     }
}

static void __max7219_leftShiftInitialBuffer(void) {
     for(uint8_t column = 0; column < 9; column++)
         max7219.screenBuffer[0][column] >>= 1;
}

static void __max7219_shiftBuffer(void) {  
     __max7219_leftShiftScreenBuffer();
     __max7219_loadInitialBufferToFirstScreenBuffer();
     __max7219_loadScreenBuffers();
     __max7219_leftShiftInitialBuffer();
}

static void __max7219_trimBufferLeft (uint8_t* buffer) {
       for(int trim_count=0; trim_count<2; trim_count++) {
               for(int arr=0; arr<8; arr++)
                   buffer[arr] >>=1;
       }
}

static void __max7219_shiftArrayBuffer (uint8_t* buffer) {
        for(uint8_t row=0; row<MAX7219_ROW_SIZE; row++)
            max7219.screenBuffer[0][row] = buffer[row];
}

void max7219_printCharacter(uint8_t character, uint8_t speed) {
     uint8_t arrBuffer[8] = {0};
     __max7219_getPrintableBuffer(character, (uint8_t*)&arrBuffer);
     __max7219_trimBufferLeft((uint8_t*)&arrBuffer);
     __max7219_shiftArrayBuffer(arrBuffer);   
     for(int cnt=0; cnt < (FONT_COLUMN+1); cnt++) { 
        for(int scan = 0; scan < speed; scan++)
             __max7219_writePixel();   
         __max7219_shiftBuffer();
     } 
     
}

void max7219_printText(uint8_t* text) {
     int string_len = 0;
     while(text[string_len] != '\0') {
        max7219_printCharacter(text[string_len], 1);
        string_len++;
     }
}

void max7219_scrollText(uint8_t* text, uint8_t speed) {
    int string_len = 0;
     while(text[string_len] != '\0') {
        max7219_printCharacter(text[string_len], speed);
        string_len++;
     }
}



