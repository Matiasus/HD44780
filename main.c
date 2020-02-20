/**
 * HD44780 Driver Communication
 *
 * Copyright (C) 2019 Marian Hrinko.
 * Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       25.11.2019
 * @file        hd44780.c
 * @tested      AVR Atmega16
 * @description Main file for test library LCD with HD44780 driver 
 */

// include libraries
#include <stdio.h>
#include <util/delay.h>
#include <avr/io.h>
#include "lib/hd44780.h"

/**
 * @desc    Main function
 *
 * @param   void
 * @return  int
 */
int main (void)
{
  // init diplay in 4 bit mode
  HD44780_Init();
  // display clear
  HD44780_DisplayClear();
  // set position
  HD44780_PositionXY(6, 0);
  // send char
  HD44780_DrawString("BOOM");
  // display clear
  HD44780_CursorOn();

  // return value
  return SUCCESS;
}
