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
  HD44780_Init(e4BIT);
  // cursor blink
  HD44780_DisplayOn();

  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, ' ');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, ' ');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'T');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'I');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'N');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'K');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'A');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, ' ');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, '&');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, ' ');
  // second line 0xC0 - send 8 bits in 4 bit mode
//  HD44780_SendInstruction(HD44780_Send8bitsIn4bitMode, 0xC0);
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'A');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'D');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'K');
  // send char
  HD44780_SendData(HD44780_Send8bitsIn4bitMode, 'O');

  // return value
  return 0;
}
