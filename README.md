# C-library for HD44780 LCD driver

## HD44780 Description
Detailed information is in [HD44780 Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf).

## Library
Library is aimed for MCU ATmega16 / Atmega8 which supports 4-bit.
- [4-bit Operation](#4-bit-operation)

### Usage
Prior defined for Atmega16 / Atmega8.

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
## Functions

- [HD44780_Init()](#hd44780_init) - init display
- [HD44780_DisplayClear()](#hd44780_displayclear) - clear display and set position to 0, 0
- [HD44780_DisplayOn()](#hd44780_displayon) - turn on display
- [HD44780_CursorOn()](#hd44780_cursoron) - turn on cursor
- [HD44780_CursorBlink()](#hd44780_cursorblink) - blink the cursor blink
- [HD44780_DrawChar(char character)](#hd44780_drawchar) - draw character on display
- [HD44780_DrawString(char * str)](#hd44780_drawstring) - draw string
- [HD44780_PositionXY(char x, char y)](#hd44780_positionxy) - set position X, Y
- [HD44780_Shift(char item, char direction)](#hd44780_shift) - shift cursor or display to left or right

### HD44780_Init
```c
void HD44780_Init (void)
```
Base initialisation function. If the electrical characteristics conditions listed under the table Power Supply Conditions Using
Internal Reset Circuit are not met, the internal reset circuit will not operate normally and will fail to initialize the HD44780U. For such a case, initialization must be performed by the MPU as explained in the section [4-bit Operation](#4-bit-operation) or [8-bit Operation](#8-bit-operation) depending on mode.

### HD44780_DisplayClear
```c
void HD44780_DisplayClear (void)
```
Display clear and set cursor to position 0, 0.

### HD44780_DisplayOn
```c
void HD44780_DisplayOn (void)
```
Turn on the display.

### HD44780_CursorOn
```c
void HD44780_CursorOn (void)
```
Turn on the cursor. Cursor will be visible.

### HD44780_CursorBlink
```c
void HD44780_CursorBlink (void)
```
Turn the cursor blink. Cursor will be visible and it will blink.

### HD44780_DrawChar
```c
void HD44780_DrawChar (char character)
```
Draw specific char on display according to ANSI table.

### HD44780_DrawString
```c
void HD44780_DrawString (char *str)
```
Draw string.

### HD44780_PositionXY
```c
char HD44780_PositionXY (char x, char y)
```
Set DDRAM or CGRAM on specific position X, Y. For LCD 16x2 (cols, rows) maximal possible values:
- X from interval values {0; 1; ... 16},
- Y from interval values {0; 1}.

### HD44780_Shift
```c
char HD44780_Shift (char item, char direction)
```
Shift cursor or display to left or right.
Item defines either cursor or display we want to move. Two possible values for item are defined:
- HD44780_CURSOR, 
- HD44780_DISPLAY.

The second parameter is direction definition. Two possible values for direction are defined:
- HD44780_RIGHT,
- HD44780_LEFT.

# Links
[HD44780 Datasheet](https://www.sparkfun.com/datasheets/LCD/HD44780.pdf)
