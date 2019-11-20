/**
 * HD44780 Driver Communication
 *
 * Copyright (C) 2019 Marian Hrinko.
 * Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       05.11.2019
 * @file        hd44780.c
 * @tested      AVR Atmega16 / AVR Atmega8
 * @inspiration
 * ---------------------------------------------------
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "hd44780.h"

// Initializing LCD Driver HD44780 according to Figure 24
// in https://www.sparkfun.com/datasheets/LCD/HD44780.pdf
//
// -----------------------------
// |         Power on          |
// -----------------------------
//              |
// -----------------------------
// | Wait for more than 15 ms  |
// | after VCC rises to 4.5 V  |
// -----------------------------
//              |
// -----------------------------
// |  RS R/W DB7 DB6 DB5 DB4   |
// |   0   0   0   0   1   1   |
// -----------------------------
//              |
// -----------------------------
// | Wait for more than 4.1 ms |
// -----------------------------
//              |
// -----------------------------
// |  RS R/W DB7 DB6 DB5 DB4   |
// |   0   0   0   0   1   1   |
// -----------------------------
//              |
// -----------------------------
// | Wait for more than 0.1 ms |
// -----------------------------
//              |
// -----------------------------
// |  RS R/W DB7 DB6 DB5 DB4   |
// |   0   0   0   0   1   1   |
// -----------------------------
//              |
// -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
// | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
// -----------------------------   // 37 us + 4 us = 41 us
//              |
// -----------------------------   // 4 bit operation
// |  RS R/W DB7 DB6 DB5 DB4   |   // 0x20
// |   0   0   0   0   1   0   |
// -----------------------------
//              |
// -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
// | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
// -----------------------------   // 37 us + 4 us = 41 us
//              |
// -----------------------------   // Display off
// |  RS R/W DB7 DB6 DB5 DB4   |   //
// |   0   0   0   0   0   0   |   // D - display: 0 - no, 1 - yes
// |   0   0   1   D   C   B   |   // C - cursor: 0 - no, 1 - yes
// -----------------------------   // B - blink cursor: 0 - no, 1 - yes
//              |
// -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
// | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
// -----------------------------   // 37 us + 4 us = 41 us
//              |
// -----------------------------   // Display clear
// |  RS R/W DB7 DB6 DB5 DB4   |   // 0x01
// |   0   0   0   0   0   0   |
// |   0   0   0   0   0   1   |
// -----------------------------
//              |
// -----------------------------
// | Wait for more than 41 us  |
// -----------------------------
//              |
// -----------------------------   // Entry mode set
// |  RS R/W DB7 DB6 DB5 DB4   |   //
// |   0   0   0   0   0   0   |   // I/D - shift cursor  (1 - left, 0 - right)
// |   0   0   0   1 I/D   S   |   // S - shift text: 1 - no, 0 - yes
// -----------------------------
//              |
// -----------------------------
// | Wait for more than 41 us  |
// -----------------------------

unsigned short int HD44780_mode = e4BIT;

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   EMode
 * @return  void
 */
void HD44780_Init (EMode mode)
{
  // set E as output
  SETBIT(HD44780_DDR_E, HD44780_E);
  // set RS as output
  SETBIT(HD44780_DDR_RS, HD44780_RS);
  // set RW as output
  SETBIT(HD44780_DDR_RW, HD44780_RW);

  // set DB7-DB4 as output
  SETBIT(HD44780_DDR_DB, HD44780_DB7);
  SETBIT(HD44780_DDR_DB, HD44780_DB6);
  SETBIT(HD44780_DDR_DB, HD44780_DB5);
  SETBIT(HD44780_DDR_DB, HD44780_DB4);

  // delay
  _delay_ms(15);  
  
  // clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // clear RW
  CLRBIT(HD44780_PORT_RW, HD44780_RW);
  // clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);

  // Busy Flag (BF) cannot be checked before these instructions
  // ---------------------------------------------------------------------
  // Initial sequence 0x30 - send 4 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send4bitsIn4bitMode, HD44780_INIT_SEQ);
  // delay > 4.1ms
  _delay_ms(5);

  // pulse E
  HD44780_PulseE();
  // delay > 100us
  _delay_us(200);

  // pulse E
  HD44780_PulseE();
  // delay > 45us (=37+4 * 270/250)
  _delay_us(50);
  // ----------------------------------------------------------------------
  
  // 4 bit mode 0x20 - send 4 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send4bitsIn4bitMode, HD44780_4BIT_MODE);

  // 4-bit & 2-lines & 5x8-dots 0x28 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_4BIT_MODE | HD44780_2_ROWS);

  // display off 0x08 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_DISP_OFF);

  // display clear 0x01 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_DISP_CLEAR);

  // entry mode set 0x06 - send 8 bits in 4 bit mode
  HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_ENTRY_MODE);

}

/**
 * @desc    Busy flag read
 *
 * @param   void
 * @return  void
 */
char HD44780_ReadBF (void)
{
  // clr DB7-DB4 as input
  CLRBIT(HD44780_DDR_DB, HD44780_DB7);
  CLRBIT(HD44780_DDR_DB, HD44780_DB6);
  CLRBIT(HD44780_DDR_DB, HD44780_DB5);
  CLRBIT(HD44780_DDR_DB, HD44780_DB4);

  // clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // set RW - read BF
  SETBIT(HD44780_PORT_RW, HD44780_RW);

  // loop until PIN DB7 is not 1
  while ((HD44780_PIN_DB & (1 << HD44780_DB7)));

  // set DB7-DB4 as output
  SETBIT(HD44780_DDR_DB, HD44780_DB7);
  SETBIT(HD44780_DDR_DB, HD44780_DB6);
  SETBIT(HD44780_DDR_DB, HD44780_DB5);
  SETBIT(HD44780_DDR_DB, HD44780_DB4);
}

/**
 * @desc    LCD display clear
 *
 * @param   void
 * @return  void
 */
void HD44780_DisplayClear (void)
{
  // 4 bit mode
  if (HD44780_mode == e4BIT) {
    // Diplay clear - send 8 bits in 4 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_DISP_CLEAR);
  // 8 bit mode
  } else {
    // Diplay clear - send 8 bits in 8 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn8bitMode, HD44780_DISP_CLEAR);
  }
}

/**
 * @desc    LCD display on
 *
 * @param   void
 * @return  void
 */
void HD44780_DisplayOn (void)
{
  // 4 bit mode
  if (HD44780_mode == e4BIT) {
    // Diplay on - send 8 bits in 4 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_DISP_ON);
  // 8 bit mode
  } else {
    // Diplay on - send 8 bits in 8 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn8bitMode, HD44780_DISP_ON);
  }
}

/**
 * @desc    LCD cursor on, display on
 *
 * @param   void
 * @return  void
 */
void HD44780_CursorOn (void)
{
  // 4 bit mode
  if (HD44780_mode == e4BIT) {
    // Cursor on - send 8 bits in 4 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_CURSOR_ON);
  // 8 bit mode
  } else {
    // Cursor on - send 8 bits in 8 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn8bitMode, HD44780_CURSOR_ON);
  }
}

/**
 * @desc    LCD cursor blink, cursor on, display on
 *
 * @param   void
 * @return  void
 */
void HD44780_CursorBlink (void)
{
  // 4 bit mode
  if (HD44780_mode == e4BIT) {
    // Cursor blink - send 8 bits in 4 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, HD44780_CURSOR_BLINK);
  // 8 bit mode
  } else {
    // Cursor blink - send 8 bits in 8 bit mode
    HD44780_SendInstruction(HD44780_Send8bitsIn8bitMode, HD44780_CURSOR_BLINK);
  }
}

/**
 * @desc    LCD send instruction
 *
 * @param   void (*function) (unsigned short int)
 * @return  void
 */
void HD44780_SendInstruction (void (*SendBitMode) (unsigned short int data), unsigned short int data)
{
  // Clear RS
  HD44780_PORT_RS &= ~(1 << HD44780_RS);
  // send required data in required mode
  SendBitMode(data);
  // delay
  _delay_ms(1);
}

/**
 * @desc    LCD send data
 *
 * @param   void (*function) (unsigned short int)
 * @return  void
 */
void HD44780_SendData (void (*SendBitMode) (unsigned short int data), unsigned short int data)
{
  // Clear RS
  HD44780_PORT_RS |= (1 << HD44780_RS);
  // send required data in required mode
  SendBitMode(data);
  // Clear RS
  HD44780_PORT_RS &= ~(1 << HD44780_RS);
  // delay
  _delay_us(40);  
}

/**
 * @desc    LCD send 4bits instruction in 4 bit mode
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_Send4bitsIn4bitMode (unsigned short int data)
{
  // send data to LCD
  HD44780_SetUppNibble(data);
  // pulse E
  HD44780_PulseE();
}

/**
 * @desc    LCD send 8bits instruction in 4 bit mode
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_Send8bitsIn4bitMode (unsigned short int data)
{
  // send data to LCD
  HD44780_SetUppNibble(data);
  // pulse E
  HD44780_PulseE();
  // delay
  //_delay_us(1);
  // send data to LCD
  HD44780_SetUppNibble(data << 4);
  // pulse E
  HD44780_PulseE();
}

/**
 * @desc    LCD send 8bits instruction in 8 bit mode
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_Send8bitsIn8bitMode (unsigned short int data)
{
  // send data to LCD
  HD44780_SetUppNibble(data);
  // send data to LCD
  HD44780_SetLowNibble(data);
  // pulse E
  HD44780_PulseE();
}

/**
 * @desc    LCD send upper nibble
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SetUppNibble (unsigned short int data)
{
  // clear bits DB7-DB4
  CLRBIT(HD44780_PORT_DB, HD44780_DB7);
  CLRBIT(HD44780_PORT_DB, HD44780_DB6);
  CLRBIT(HD44780_PORT_DB, HD44780_DB5);
  CLRBIT(HD44780_PORT_DB, HD44780_DB4);
  // set DB7-DB4 if corresponding bit is set
  if (data & 0x80) { SETBIT(HD44780_PORT_DB, HD44780_DB7); }
  if (data & 0x40) { SETBIT(HD44780_PORT_DB, HD44780_DB6); }
  if (data & 0x20) { SETBIT(HD44780_PORT_DB, HD44780_DB5); }
  if (data & 0x10) { SETBIT(HD44780_PORT_DB, HD44780_DB4); }
}

/**
 * @desc    LCD send lower nibble
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SetLowNibble (unsigned short int data)
{
/*
  // clear bits DB7-DB4
  HD44780_PORT_DB &= ~(1 << HD44780_DB3);
  HD44780_PORT_DB &= ~(1 << HD44780_DB2);
  HD44780_PORT_DB &= ~(1 << HD44780_DB1);
  HD44780_PORT_DB &= ~(1 << HD44780_DB0);
  // set DB7-DB4 if corresponding bit is set
  if (data & BIT3) { HD44780_PORT_DB |= (1 << HD44780_DB3); }
  if (data & BIT2) { HD44780_PORT_DB |= (1 << HD44780_DB2); }
  if (data & BIT1) { HD44780_PORT_DB |= (1 << HD44780_DB1); }
  if (data & BIT0) { HD44780_PORT_DB |= (1 << HD44780_DB0); }
*/
}

/**
 * @desc    LCD pulse E
 *
 * @param   void
 * @return  void
 */
void HD44780_PulseE (void)
{
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // clock impuls > 0.5us
  _delay_us(0.6);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
}
