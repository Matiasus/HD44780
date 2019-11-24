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
#include "lib/hd44780.h"

/**
 * @desc    Main
 *
 * @param   void
 * @return  void
 */
int main (void)
{
  // init diplay in 4 bit mode
  HD44780_Init();

  // display clear
  HD44780_DisplayClear();
  // send char
  HD44780_DrawString("I LOVE YOU TINKA");
  // set position
  HD44780_PositionXY(5, 1);
  // send char
  HD44780_DrawString("MARIAN");
  // display clear
  HD44780_CursorOn();

  _delay_ms(1000);
  
  // shift cursor to right
  HD44780_Shift(HD44780_CURSOR, HD44780_LEFT);

  _delay_ms(1000);
  
  // shift cursor to right
  HD44780_Shift(HD44780_CURSOR, HD44780_RIGHT);

  _delay_ms(1000);
  
  // shift cursor to right
  HD44780_Shift(HD44780_DISPLAY, HD44780_LEFT);

  _delay_ms(1000);
  
  // shift cursor to right
  HD44780_Shift(HD44780_DISPLAY, HD44780_RIGHT);


  // return value
  return 0;
}
