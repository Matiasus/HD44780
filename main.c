/** 
 * ---------------------------------------------------+ 
 * @desc        Main file
 * ---------------------------------------------------+ 
 * @copyright   Copyright (C) 2020 Marian Hrinko.
 * @author      Marian Hrinko
 * @email       mato.hrinko@gmail.com
 * @datum       10.11.2020
 * @update      15.04.2021
 * @file        main.c
 * @version     1.0
 * @tested      AVR Atmega16a
 * ---------------------------------------------------+
 */

// include libraries
#include <util/delay.h>
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

  // DISPALY ON
  // --------------------------
  // display clear
  HD44780_DisplayClear();
  // set position
  HD44780_PositionXY(0, 0);
  // send char
  HD44780_DrawString("DISPLAY ON");
  // display clear
  HD44780_DisplayOn();
  // delay
  _delay_ms(2000);

  // CURSOR ON & DISPLAY ON
  // --------------------------
  // display clear
  HD44780_DisplayClear();
  // set position
  HD44780_PositionXY(0, 0);
  // send char
  HD44780_DrawString("CURSOR ON");
  // cursor on
  HD44780_CursorOn();
  // delay
  _delay_ms(2000);

  // CURSOR BLINK & DISPLAY ON
  // --------------------------
  // display clear
  HD44780_DisplayClear();
  // set position
  HD44780_PositionXY(0, 0);
  // send char
  HD44780_DrawString("CURSOR BLINK");
  // delay
  HD44780_CursorBlink();
  // delay
  _delay_ms(2000);

  // CURSOR OFF & DISPLAY ON
  // --------------------------
  // display clear
  HD44780_DisplayClear();
  // set position
  HD44780_PositionXY(0, 0);
  // send char
  HD44780_DrawString("CURSOR OFF");
  // delay
  HD44780_CursorOff();

  // EXIT
  // ------------------------------------------------- 
  // return & exit
  return 0;
}
