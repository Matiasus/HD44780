# C library for HD44780 LCD driver

## HD44780 Description
A single HD44780U can display up to one 8-character line or two 8-character lines. The HD44780U has pin function compatibility with the HD44780S which allows the user to easily replace an LCD-II with an HD44780U. The HD44780U character generator ROM is extended to generate 208 5x8 dot character fonts and 32 5x10 dot character fonts for a total of 240 different character fonts.

The low power supply (2.7V to 5.5V) of the HD44780U is suitable for any portable battery-driven product requiring low power dissipation. [HD44780 Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)

## Library
Library supports 4-bit and 8-bit communication with HD44780 LCD driver. Detail description how to interface with MCU:
- [4-bit Operation](#4-bit-operation)
- [8-bit Operation](#8-bit-operation)

### 4-bit Operation

Initializing LCD Driver HD44780 according to Figure 24 in [HD44780 Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf).
 ```c
// +---------------------------+
// |         Power on          |
// | Wait for more than 15 ms  |   // 15 ms wait
// | after VCC rises to 4.5 V  |
// +---------------------------+
//              |
// +---------------------------+ 
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 4.1 ms |   // 4.1 ms us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   
// |   0   0   0   0   1   1   |   // Initial sequence 0x30
// | Wait for more than 0.1 ms |   // 100 us writing DATA into DDRAM or CGRAM
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Initial sequence 0x30
// |   0   0   0   0   1   1   |   // 37 us writing DATA into D(C)DRAM 4us tadd - time after BF disapeared
// | Wait for more than 45 us  |   // 37 us + 4 us = 41 us * (270/250) = 45us
// +---------------------------+  
//              |
// +---------------------------+   // 4bit mode 0x20
// |  RS R/W DB7 DB6 DB5 DB4   |   // 
// |   0   0   0   0   1   0   |   // 37 us writing DATA into D(C)DRAM 4us tadd - time after BF disapeared
// | Wait for more than 45 us  |   // !!! MUST BE SET DELAY TIME, BUSY FLAG CHECK DOESN'T WORK CORRECTLY !!!
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display off 0x08
// |   0   0   0   0   0   0   |   // 
// |   0   0   1   0   0   0   |   // 
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display clear 0x01
// |   0   0   0   0   0   0   |   //
// |   0   0   0   0   0   1   |   //
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Entry mode set 0x06
// |   0   0   0   0   0   0   |   // 
// |   0   0   0   1   1   0   |   // shift cursor to the left, without text shifting
// |    Wait for BF Cleared    |   // Wait for BF Cleared
// +---------------------------+
```
### Functions

- [HD44780_Init()](#HD44780_Init)
- [HD44780_DisplayClear()](#HD44780_DisplayClear)
- [HD44780_DisplayOn ()](#HD44780_DisplayOn)
- [HD44780_CursorOn ()](#HD44780_Init)
- [HD44780_CursorBlink ()](#HD44780_Init)
- [HD44780_DrawChar (char character)](#HD44780_Init)
- [HD44780_DrawString (char * str)](#HD44780_Init)
- [HD44780_PositionXY (char x, char y)](#HD44780_Init)
- [HD44780_Shift (char item, char direction)](#HD44780_Init)

# Links
[HD44780 Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)
