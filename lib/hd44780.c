/** 
 * ---------------------------------------------------------------+ 
 * @desc        HD44780 LCD Driver 
 * ---------------------------------------------------------------+ 
 *              Copyright (C) 2020 Marian Hrinko.
 *              Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       18.11.2020
 * @file        hd44780.c
 * @tested      AVR Atmega16a
 *
 * @depend      hd44780.h
 * ---------------------------------------------------------------+
 * @usage       default set 16x2 LCD
 *              4-bit with 3 control wires (RW, RS, E)
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "hd44780.h"

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
// |   0   0   0   0   1   1   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // 37 us + 4 us = 41 us * (270/250) = 45us
// +---------------------------+  
//              |
// +---------------------------+   // 4bit mode 0x20 !!! MUST BE SET TIME, BF CHECK DOESN'T WORK CORRECTLY !!!
// |  RS R/W DB7 DB6 DB5 DB4   |   // 
// |   0   0   0   0   1   0   |   // 37 us writing DATA into DDRAM or CGRAM 4 us tadd - time after busy flag disapeared
// | Wait for more than 45 us  |   // !!! MUST BE SET DELAY TIME, BUSY FLAG CHECK DOESN'T WORK CORRECTLY !!!
// +---------------------------+
//              |
// +---------------------------+
// |  RS R/W DB7 DB6 DB5 DB4   |   // Display off 0x08
// |   0   0   0   0   1   0   |   // 
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

/**
 * @desc    LCD display clear
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_DisplayClear (void)
{
  // Diplay clear
  HD44780_SendInstruction(HD44780_DISP_CLEAR);
}

/**
 * @desc    LCD display on
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_DisplayOn (void)
{
  // send instruction - display on
  HD44780_SendInstruction(HD44780_DISP_ON);
}

/**
 * @desc    LCD cursor on, display on
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_CursorOn (void)
{
  // send instruction - cursor on
  HD44780_SendInstruction(HD44780_CURSOR_ON);
}

/**
 * @desc    LCD cursor off
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_CursorOff (void)
{
  // send instruction - cursor on
  HD44780_SendInstruction(HD44780_CURSOR_OFF);
}

/**
 * @desc    LCD cursor blink, cursor on, display on
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_CursorBlink (void)
{
  // send instruction - Cursor blink
  HD44780_SendInstruction(HD44780_CURSOR_BLINK);
}

/**
 * @desc    LCD draw char
 *
 * @param   char
 *
 * @return  void
 */
void HD44780_DrawChar (char character)
{
  // Diplay clear
  HD44780_SendData(character);
}

/**
 * @desc    LCD draw string
 *
 * @param   char *
 *
 * @return  void
 */
void HD44780_DrawString (char *str)
{
  unsigned char i = 0;
  // loop through 5 bytes
  while (str[i] != '\0') {
    //read characters and increment index
    HD44780_SendData(str[i++]);
  }
}

/**
 * @desc    Got to position x,y
 *
 * @param   char
 * @param   char
 *
 * @return  char
 */
char HD44780_PositionXY (char x, char y)
{
  if (x > HD44780_COLS || y > HD44780_ROWS) {
    // error
    return ERROR;
  }
  // check which row
  if (y == 0) {
    // send instruction 1st row
    HD44780_SendInstruction(HD44780_POSITION | (HD44780_ROW1_START + x));
  } else if (y == 1) {
    // send instruction 2nd row
    HD44780_SendInstruction(HD44780_POSITION | (HD44780_ROW2_START + x));
  }
  // success
  return 0;
}

/**
 * @desc    Shift cursor / display to left / right
 *
 * @param   char item {HD44780_CURSOR; HD44780_DISPLAY}
 * @param   char direction {HD44780_RIGHT; HD44780_LEFT}
 *
 * @return  char
 */
char HD44780_Shift (char item, char direction)
{
  // check if item is cursor or display or direction is left or right
  if ((item != HD44780_DISPLAY) && (item != HD44780_CURSOR)) {
    // error
    return ERROR;
  }
  // check if direction is left or right
  if ((direction != HD44780_RIGHT) && (direction != HD44780_LEFT)) {
    // error
    return ERROR;
  }

  // cursor shift
  if (item == HD44780_CURSOR) {
    // right shift
    if (direction == HD44780_RIGHT) {
      // shit cursor / display to right / left
      HD44780_SendInstruction(HD44780_SHIFT | HD44780_CURSOR | HD44780_RIGHT);
    } else {
      // shit cursor / display to right / left
      HD44780_SendInstruction(HD44780_SHIFT | HD44780_CURSOR | HD44780_LEFT);
    }
  // display shift
  } else {
    // right shift
    if (direction == HD44780_RIGHT) {
      // shit cursor / display to right / left
      HD44780_SendInstruction(HD44780_SHIFT | HD44780_DISPLAY | HD44780_RIGHT);
    } else {
      // shit cursor / display to right / left
      HD44780_SendInstruction(HD44780_SHIFT | HD44780_DISPLAY | HD44780_LEFT);
    }
  }
  // success
  return 0;
}

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_Init (void)
{
  // set E as output
  SETBIT(HD44780_DDR_E, HD44780_E);
  // set RS as output
  SETBIT(HD44780_DDR_RS, HD44780_RS);
  // set RW as output
  SETBIT(HD44780_DDR_RW, HD44780_RW);

  // set DB7-DB4 as output
  HD44780_SetDDR_DATA4to7();
  
  // clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // clear RW
  CLRBIT(HD44780_PORT_RW, HD44780_RW);
  // clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);

  // delay > 15ms
  _delay_ms(16);

  // Busy Flag (BF) cannot be checked in these instructions
  // ---------------------------------------------------------------------
  // Initial sequence 0x30 - send 4 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_INIT_SEQ);
  // delay > 4.1ms
  _delay_ms(5);

  // pulse E
  HD44780_PulseE();
  // delay > 100us
  _delay_us(110);

  // pulse E
  HD44780_PulseE();
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);

  // 4 bit mode 0x20 - send 4 bits in 4 bit mode
  HD44780_Send4bitsIn4bitMode(HD44780_4BIT_MODE);
  // pulse E
  HD44780_PulseE();
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);
  // ----------------------------------------------------------------------
  
  // 4-bit & 2-lines & 5x8-dots 0x28 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_4BIT_MODE | HD44780_2_ROWS | HD44780_FONT_5x8);

  // display off 0x08 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_DISP_OFF);

  // display clear 0x01 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_DISP_CLEAR);

  // entry mode set 0x06 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_ENTRY_MODE);
}

/**
 * @desc    Check Busy Flag (BF) in 4 bit mode
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_CheckBFin4bitMode (void)
{
  unsigned char input = 0;

  // clear DB7-DB4 as input
  HD44780_ClearDDR_DATA4to7();
  // set pull-up resistors for DB7-DB4 
  HD44780_SetPORT_DATA4to7();

  // clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // set RW - read instruction
  SETBIT(HD44780_PORT_RW, HD44780_RW);

  // test HIGH level on PIN DB7
  // after clear PIN DB7 should continue
  // -------------------------------------
  //  us:     0.5|0.5|0.5
  //          ___     ___
  //   E: ___/   \___/   \__
  //           ___     ___
  // DB7: \___/   \___/   \__
  //
  while (1) {

    // Read upper nibble
    // --------------------------------
    // Set E
    SETBIT(HD44780_PORT_E, HD44780_E);
    // PWeh > 0.5us
    _delay_us(0.5);
    // read upper nibble (tDDR > 360ns)
    input = HD44780_PIN_DATA;
    // Clear E
    CLRBIT(HD44780_PORT_E, HD44780_E);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    _delay_us(0.5);

    // Read lower nibble
    // --------------------------------
    // Set E
    SETBIT(HD44780_PORT_E, HD44780_E);
    // PWeh > 0.5us
    _delay_us(0.5);
    // read lower nibble (tDDR > 360ns)
    input |= HD44780_PIN_DATA >> 4;
    // Clear E
    CLRBIT(HD44780_PORT_E, HD44780_E);
    // TcycE > 1000ns -> delay depends on PWeh delay time
    // delay = TcycE - PWeh = 1000 - 500 = 500ns
    _delay_us(0.5);

    // check if DB7 is cleared
    if (!(input & (1 << HD44780_DATA7))) {
      // if BF cleared -> end loop
      break;
    }
  }

  // clear RW
  CLRBIT(HD44780_PORT_RW, HD44780_RW);

  // set DB7-DB4 as output
  HD44780_SetDDR_DATA4to7();
}

/**
 * @desc    Check Busy Flag (BF) in 8 bit mode
 *
 * @param   void
 * @return  void
 */
void HD44780_CheckBFin8bitMode (void)
{
}

/**
 * @desc    LCD send instruction
 *
 * @param   unsigned short int 
 *
 * @return  void
 */
void HD44780_SendInstruction (unsigned short int data)
{
  // Clear RS
  HD44780_PORT_RS &= ~(1 << HD44780_RS);

  // 4bit mode
  // ------------------------------------------
  if (HD44780_MODE == HD44780_4BIT_MODE) {
    // send required data in required mode
    HD44780_Send8bitsIn4bitMode(data);
    // check busy flag
    HD44780_CheckBFin4bitMode();
  // 8 bit mode
  // ------------------------------------------
  } else if (HD44780_MODE == HD44780_8BIT_MODE) {
    // send required data in required mode
    HD44780_Send8bitsIn8bitMode(data);
    // check busy flag
    HD44780_CheckBFin8bitMode();
  }
}

/**
 * @desc    LCD send data
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_SendData (unsigned short int data)
{
  // Set RS
  SETBIT(HD44780_PORT_RS, HD44780_RS);

  // 4bit mode
  // ------------------------------------------
  if (HD44780_MODE == HD44780_4BIT_MODE) {
    // send required data in required mode
    HD44780_Send8bitsIn4bitMode(data);
    // check busy flag
    HD44780_CheckBFin4bitMode();
  // 8 bit mode
  // ------------------------------------------
  } else if (HD44780_MODE == HD44780_8BIT_MODE) {
    // send required data in required mode
    HD44780_Send8bitsIn8bitMode(data);
    // check busy flag
    HD44780_CheckBFin8bitMode();
  }

  // Clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS); 
}

/**
 * @desc    LCD send 4bits instruction in 4 bit mode
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_Send4bitsIn4bitMode (unsigned short int data)
{
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // send data to LCD
  HD44780_SetUppNibble(data);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
  // TcycE > 1000ns -> delay depends on PWeh delay time
  // delay = TcycE - PWeh = 1000 - 500 = 500ns
  _delay_us(0.5);
}

/**
 * @desc    LCD send 8bits instruction in 4 bit mode
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_Send8bitsIn4bitMode (unsigned short int data)
{
  // Send upper nibble
  // ----------------------------------
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // send data to LCD
  HD44780_SetUppNibble(data);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
  // TcycE > 1000ns -> delay depends on PWeh delay time
  // delay = TcycE - PWeh = 1000 - 500 = 500ns
  _delay_us(0.5);

  // Send lower nibble
  // ----------------------------------
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // send data to LCD
  HD44780_SetUppNibble(data << 4);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
  // TcycE > 1000ns -> delay depends on PWeh delay time
  // delay = TcycE - PWeh = 1000 - 500 = 500ns
  _delay_us(0.5);
}

/**
 * @desc    LCD send 8bits instruction in 8 bit mode
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_Send8bitsIn8bitMode (unsigned short int data)
{
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // send data to LCD
  HD44780_SetUppNibble(data);
  // send data to LCD
  HD44780_SetLowNibble(data);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
  // TcycE > 1000ns -> delay depends on PWeh delay time
  // delay = TcycE - PWeh = 1000 - 500 = 500ns
  _delay_us(0.5);
}

/**
 * @desc    LCD send upper nibble
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_SetUppNibble (unsigned short int data)
{
  // clear bits DB7-DB4
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA7);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA6);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA5);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA4);
  // set DB7-DB4 if corresponding bit is set
  if (data & 0x80) { SETBIT(HD44780_PORT_DATA, HD44780_DATA7); }
  if (data & 0x40) { SETBIT(HD44780_PORT_DATA, HD44780_DATA6); }
  if (data & 0x20) { SETBIT(HD44780_PORT_DATA, HD44780_DATA5); }
  if (data & 0x10) { SETBIT(HD44780_PORT_DATA, HD44780_DATA4); }
}

/**
 * @desc    LCD send lower nibble
 *
 * @param   unsigned short int
 *
 * @return  void
 */
void HD44780_SetLowNibble (unsigned short int data)
{
  // clear bits DB7-DB4
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA3);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA2);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA1);
  CLRBIT(HD44780_PORT_DATA, HD44780_DATA0);
  // set DB7-DB4 if corresponding bit is set
  if (data & 0x08) { SETBIT(HD44780_PORT_DATA, HD44780_DATA3); }
  if (data & 0x04) { SETBIT(HD44780_PORT_DATA, HD44780_DATA2); }
  if (data & 0x02) { SETBIT(HD44780_PORT_DATA, HD44780_DATA1); }
  if (data & 0x01) { SETBIT(HD44780_PORT_DATA, HD44780_DATA0); }
}

/**
 * @desc    LCD pulse E
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_PulseE (void)
{
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // PWeh delay time > 450ns
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
  // TcycE > 1000ns -> delay depends on PWeh delay time
  // delay = TcycE - PWeh = 1000 - 500 = 500ns
  _delay_us(0.5);
}

/**
 * @desc    Set PORT DB4 to DB7
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_SetPORT_DATA4to7 (void)
{
  // set DB4-DB7  
  SETBIT(HD44780_PORT_DATA, HD44780_DATA4);
  SETBIT(HD44780_PORT_DATA, HD44780_DATA5);
  SETBIT(HD44780_PORT_DATA, HD44780_DATA6);
  SETBIT(HD44780_PORT_DATA, HD44780_DATA7);
}

/**
 * @desc    Clear DDR DB4 to DB7
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_ClearDDR_DATA4to7 (void)
{
  // set DB4-DB7  
  CLRBIT(HD44780_DDR_DATA, HD44780_DATA4);
  CLRBIT(HD44780_DDR_DATA, HD44780_DATA5);
  CLRBIT(HD44780_DDR_DATA, HD44780_DATA6);
  CLRBIT(HD44780_DDR_DATA, HD44780_DATA7);
}

/**
 * @desc    Set DDR DB4 to DB7
 *
 * @param   void
 *
 * @return  void
 */
void HD44780_SetDDR_DATA4to7 (void)
{
  // set DB7-DB4 as output
  SETBIT(HD44780_DDR_DATA, HD44780_DATA4);
  SETBIT(HD44780_DDR_DATA, HD44780_DATA5);
  SETBIT(HD44780_DDR_DATA, HD44780_DATA6);
  SETBIT(HD44780_DDR_DATA, HD44780_DATA7);
}
