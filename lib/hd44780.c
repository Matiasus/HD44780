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
#include <dalay.h>
#include <avr/io.h>
#include "hd44780.h"

/**
 * @desc    LCD init - initialisation routine
 *
 * @param   EMode
 * @return  void
 */
void HD44780_Init(EMode mode)
{
  // set DB7-DB4 as output
  SETBIT(HD44780_DDR_DB, DB7);
  SETBIT(HD44780_DDR_DB, DB6);
  SETBIT(HD44780_DDR_DB, DB5);
  SETBIT(HD44780_DDR_DB, DB4);
  // if 8 bit operation set DB3-DB0 as output
  if (mode == e8BIT) {
    SETBIT(HD44780_DDR_DB, HD44780_DB3);
    SETBIT(HD44780_DDR_DB, HD44780_DB2);
    SETBIT(HD44780_DDR_DB, HD44780_DB1);
    SETBIT(HD44780_DDR_DB, HD44780_DB0);
  }
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
  _delay_ms(15);
  //              |
  // -----------------------------
  // |  RS R/W DB7 DB6 DB5 DB4   |
  // |   0   0   0   0   1   1   |
  // -----------------------------
  HD44780_SendInstruction(HD44780_INIT_SEQ, mode);
  //              |
  // -----------------------------
  // | Wait for more than 4.1 ms |
  // -----------------------------
  _delay_ms(4.1);
  //              |
  // -----------------------------
  // |  RS R/W DB7 DB6 DB5 DB4   |
  // |   0   0   0   0   1   1   |
  // -----------------------------
  HD44780_SendInstruction(HD44780_INIT_SEQ, mode);
  //              |
  // -----------------------------
  // | Wait for more than 0.1 ms |
  // -----------------------------
  _delay_us(100);
  //              |
  // -----------------------------
  // |  RS R/W DB7 DB6 DB5 DB4   |
  // |   0   0   0   0   1   1   |
  // -----------------------------
  HD44780_SendInstruction(HD44780_INIT_SEQ, mode);
  //              |
  // -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
  // | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
  // -----------------------------   // 37 us + 4 us = 41 us
  _delay_us(41);
  //              |
  // -----------------------------   // 4 bit operation
  // |  RS R/W DB7 DB6 DB5 DB4   |   // 0x20
  // |   0   0   0   0   1   0   |
  // -----------------------------
  HD44780_SendInstruction(HD44780_4BIT_MODE, mode);
  //              |
  // -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
  // | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
  // -----------------------------   // 37 us + 4 us = 41 us
  _delay_us(41);
  //              |
  // -----------------------------   // Display off
  // |  RS R/W DB7 DB6 DB5 DB4   |   //
  // |   0   0   0   0   0   0   |   // D - display: 0 - no, 1 - yes
  // |   0   0   1   D   C   B   |   // C - cursor: 0 - no, 1 - yes
  // -----------------------------   // B - blink cursor: 0 - no, 1 - yes
  //  Upper nibble 0000 -> 0x00
  HD44780_SendInstruction(0x00, mode);
  //  Lower nibble 1000 -> 0x08
  HD44780_SendInstruction(0x08, mode);
  //              |
  // -----------------------------   // 37 us writing DATA into DDRAM or CGRAM
  // | Wait for more than 41 us  |   //  4 us tadd - time after busy flag disapeared
  // -----------------------------   // 37 us + 4 us = 41 us
  _delay_us(41);
  //              |
  // -----------------------------   // Display clear
  // |  RS R/W DB7 DB6 DB5 DB4   |   // 0x01
  // |   0   0   0   0   0   0   |
  // |   0   0   0   0   0   1   |
  // -----------------------------
  //  Upper nibble 0000 -> 0x00
  HD44780_SendInstruction(0x00, mode);
  //  Lower nibble 0001 -> 0x10
  HD44780_SendInstruction(0x01, mode);
  //              |
  // -----------------------------
  // | Wait for more than 41 us  |
  // -----------------------------
  _delay_us(41);
  //              |
  // -----------------------------   // Entry mode set
  // |  RS R/W DB7 DB6 DB5 DB4   |   //
  // |   0   0   0   0   0   0   |   // I/D - shift cursor  (1 - left, 0 - right)
  // |   0   0   0   1 I/D   S   |   // S - shift text: 1 - no, 0 - yes
  // -----------------------------
  //  Upper nibble 0000 -> 0x00
  HD44780_SendInstruction(0x00, mode);
  //  Lower nibble 0110 -> 0x60
  HD44780_SendInstruction(0x06, mode);
  //              |
  // -----------------------------
  // | Wait for more than 41 us  |
  // -----------------------------
  _delay_us(41);
}

/**
 * @desc    LCD send instruction
 *
 * @param   unsigned short int
 * @param   EMode
 * @return  void
 */
void HD44780_SendInstruction(unsigned short int data, EMode mode)
{
  // Clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // send data to LCD
  HD44780_SendNibbles(data, mode);
}

/**
 * @desc    LCD out data
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SendData(unsigned short int data)
{
  // Set RS
  SETBIT(HD44780_PORT_RS, HD44780_RS);
  // send data to LCD
  HD44780_SendNibbles(data, mode);
  // Clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
}

/**
 * @desc    LCD out upper and lower nibbles
 *
 * @param   unsigned short int
 * @param   EMode {e4BIT; e8BIT}
 * @return  void
 */
void HD44780_SendNibbles(unsigned short int data, EMode mode)
{
  // Send out upper nibble
  HD44780_SendUppNibble(data << mode);
  // if 8bit mode
  if (mode == e8BIT) {
    // Send out upper nibble
    HD44780_SendUppNibble(data & 0xf0);    
    // Send out lower nibble
    HD44780_SendLowNibble(data & 0x0f);
  }
  // pulse E
  HD44780_PulseE();
}

/**
 * @desc    LCD send upper nibble
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SendUppNibble(unsigned short int data)
{
  // clear bits DB7-DB4
  CLRBIT(HD44780_PORT_DB, HD44780_DB7);
  CLRBIT(HD44780_PORT_DB, HD44780_DB6);
  CLRBIT(HD44780_PORT_DB, HD44780_DB5);
  CLRBIT(HD44780_PORT_DB, HD44780_DB4);
  // set DB7-DB4 if corresponding bit is set
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB7, data, BIT7);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB6, data, BIT6);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB5, data, BIT5);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB4, data, BIT4);
}

/**
 * @desc    LCD send
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SendLowNibble(unsigned short int data)
{
  // clear bits DB3-DB0
  CLRBIT(HD44780_PORT_DB, HD44780_DB3);
  CLRBIT(HD44780_PORT_DB, HD44780_DB2);
  CLRBIT(HD44780_PORT_DB, HD44780_DB1);
  CLRBIT(HD44780_PORT_DB, HD44780_DB0);
  // set DB3-DB0 if corresponding bit is set
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB3, data, BIT3);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB2, data, BIT2);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB1, data, BIT1);
  SET_IF_BIT_IN_DATA_SET(HD44780_PORT_DB, HD44780_DB0, data, BIT0);
}

/**
 * @desc    LCD pulse E
 *
 * @param   void
 * @return  void
 */
void HD44780_PulseE(void)
{
  // Set E
  SETBIT(HD44780_PORT_E, HD44780_E);
  // clock impuls > 0.5us
  _delay_us(0.5);
  // Clear E
  CLRBIT(HD44780_PORT_E, HD44780_E);
}
