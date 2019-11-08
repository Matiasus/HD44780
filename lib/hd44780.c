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
  // -----------------------------
  // | Wait for more than 41 us  |
  // -----------------------------
  //              |
  // -----------------------------   // 4 bit operation
  // |  RS R/W DB7 DB6 DB5 DB4   |
  // |   0   0   0   0   1   0   |
  // -----------------------------
  //              |
  // -----------------------------
  // | Wait for more than 41 us  |
  // -----------------------------
  //              |
  // -----------------------------   // Display off
  // |  RS R/W DB7 DB6 DB5 DB4   |   // 
  // |   0   0   0   0   0   0   |   // D - display: 0 - no, 1 - yes
  // |   0   0   1   D   C   B   |   // C - cursor: 0 - no, 1 - yes
  // -----------------------------   // B - blink cursor: 0 - no, 1 - yes
  //              |
  // -----------------------------
  // | Wait for more than 41 us  |
  // -----------------------------
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
  
  // Wait for more than 15 ms after VCC rises to 4.5 V
  _delay_ms(15);
  // Init sequence
  HD44780_SendInstruction(HD44780_INIT_SEQ, mode);
  // Wait for more than 4.1 ms
  _delay_ms(4.1);
  // Init sequence
  HD44780_SendInstruction(0x30, mode);
  // Wait for more than 100 us
  _delay_us(100);
  // Init sequence
  HD44780_SendInstruction(0x30, mode);
  // When BF is not checked, the waiting time between instructions
  // is longer than the execution instuction time.
  //
  // 37 us writing DATA into DDRAM or CGRAM
  //  4 us tadd - time after busz flag disapeared
  // --------------------------------------------
  // t > 37 us + 4 us
  _delay_us(41);
  
  // Sequence for 4bit operation
  //  RS R/W DB7 DB6 DB5 DB4
  //   0   0   0   0   1   0
  HD44780_SendInstruction(0x20, mode);
  // Wait for more than 37 us + 4 us
  _delay_us(41);

  // Display off
  // ---------------------------------------
  // DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
  // 0   0   0   0   1   D   C   B
  //
  // D - display: 0 - no, 1 - yes
  // C - cursor: 0 - no, 1 - yes
  // B - blink cursor: 0 - no, 1 - yes
  // ---------------------------------------
  //  RS R/W DB7 DB6 DB5 DB4
  //   0   0   0   0   0   0 - upper nibble
  HD44780_SendInstruction(0x00, mode);
  //   0   0   1   0   0   0 - lower nibble
  HD44780_SendInstruction(0x80, mode);
  // Wait for more than 37 us + 4 us
  _delay_us(41);

  // Display clear
  // ---------------------------------------
  // DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
  // 0   0   0   0   0   0   0   1
  //
  // ---------------------------------------
  //  RS R/W DB7 DB6 DB5 DB4
  //   0   0   0   0   0   0 - upper nibble
  HD44780_SendInstruction(0x00, mode);
  //   0   0   0   0   0   1 - lower nibble
  HD44780_SendInstruction(0x10, mode);
  // Wait for more than 37 us + 4 us
  _delay_us(41);
  
  // Entry mode set
  // ---------------------------------------
  // DB7 DB6 DB5 DB4 DB3 DB2 DB1 DB0
  // 0   0   0   0   0   1   I/D   S
  //
  // I/D - shift cursor: 1 - left, 0 - right
  //   S - shift text: 1 - no, 0 - yes
  // ---------------------------------------
  //  RS R/W DB7 DB6 DB5 DB4
  //   0   0   0   0   0   0 - upper nibble
  HD44780_SendInstruction(0x00, mode);
  //   0   0   0   1   1   0 - lower nibble
  HD44780_SendInstruction(0x60, mode);  
  // Wait for more than 37 us + 4 us
  _delay_us(41);  
}

/**
 * @desc    LCD out upper nibble
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_OutUppNibble(unsigned short int data)
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
 * @desc    LCD out lower nibble
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_OutLowNibble(unsigned short int data)
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
 * @desc    LCD out instruction
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_OutData(unsigned short int data, EMode mode)
{
  // Send out upper nibble
  HD44780_OutUppNibble(data);
  // if 8bit mode
  if (mode == e8BIT) {
    // Send out lower nibble
    HD44780_OutLowNibble(data);
  }
  // pulse E 
  HD44780_PulseE();
}

/**
 * @desc    LCD out instruction
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SendInstruction(unsigned short int data, EMode mode)
{
  // Clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
  // send data to LCD
  HD44780_OutData(data, mode);
}

/**
 * @desc    LCD out instruction
 *
 * @param   unsigned short int
 * @return  void
 */
void HD44780_SendData(unsigned short int data)
{
  // Set RS
  SETBIT(HD44780_PORT_RS, HD44780_RS);
  // send data to LCD
  HD44780_OutData(data, mode)
  // Clear RS
  CLRBIT(HD44780_PORT_RS, HD44780_RS);
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
