/** 
 * HD44780 Driver Communication
 *
 * Copyright (C) 2019 Marian Hrinko.
 * Written by Marian Hrinko (mato.hrinko@gmail.com)
 *
 * @author      Marian Hrinko
 * @datum       05.11.2019
 * @file        hd44780.h
 * @tested      AVR Atmega16 / AVR Atmega8
 * @inspiration
 * ---------------------------------------------------
 */
#ifndef __HD44780_H__
#define __HD44780_H__

#include <avr/io.h>
#include <avr/pgmspace.h>

  // define clock
  #if defined(__AVR_ATmega8__)
    #define _FCPU 8000000
  #elif defined(__AVR_ATmega16__)
    #define _FCPU 16000000
  #endif

  #if defined(__AVR_ATmega16__)

    // E port
    #ifndef HD44780_DDR_E
      #define HD44780_DDR_E   DDRD
    #endif  
    #ifndef HD44780_PORT_E
      #define HD44780_PORT_E  PORTD
    #endif
    #ifndef HD44780_E
      #define HD44780_E       3
    #endif

    // RW port
    #ifndef HD44780_DDR_RW
      #define HD44780_DDR_RW  DDRD
    #endif  
    #ifndef HD44780_PORT_RW
      #define HD44780_PORT_RW PORTD
    #endif
    #ifndef HD44780_RW
      #define HD44780_RW      2
    #endif
    
    // RS port
    #ifndef HD44780_DDR_RS
      #define HD44780_DDR_RS  DDRD
    #endif
    #ifndef HD44780_PORT_RS
      #define HD44780_PORT_RS PORTD
    #endif    
    #ifndef HD44780_RS
      #define HD44780_RS      1
    #endif
    
    // DATA port / pin
    #ifndef HD44780_DDR_DB
      #define HD44780_DDR_DB   DDRD
    #endif
    #ifndef HD44780_PORT_DB
      #define HD44780_PORT_DB  PORTD
    #endif
    #ifndef HD44780_PIN_DB
      #define HD44780_PIN_DB   PIND
    #endif

    // port / pin 
    #ifndef HD44780_DB7    
      #define HD44780_DB7 7 // LCD PORT DB7
    #endif
    #ifndef HD44780_DB6
      #define HD44780_DB6 6 // LCD PORT DB6
    #endif
    #ifndef HD44780_DB5    
      #define HD44780_DB5 5 // LCD PORT DB5
    #endif
    #ifndef HD44780_DB4    
      #define HD44780_DB4 4 // LCD PORT DB4
    #endif
    #ifndef HD44780_DB3    
      #define HD44780_DB3 3 // LCD PORT DB3
    #endif
    #ifndef HD44780_DB2    
      #define HD44780_DB2 2 // LCD PORT DB2
    #endif
    #ifndef HD44780_DB1    
      #define HD44780_DB1 1 // LCD PORT DB1
    #endif
    #ifndef HD44780_DB0    
      #define HD44780_DB0 0 // LCD PORT DB0
    #endif   

  #endif
  
  #define BIT7 0x80
  #define BIT6 0x40
  #define BIT5 0x20
  #define BIT4 0x10
  #define BIT3 0x08
  #define BIT2 0x04
  #define BIT1 0x02
  #define BIT0 0x01
  
  #define HD44780_BUSY_FLAG    HD44780_DB7
  #define HD44780_INIT_SEQ     0x30
  #define HD44780_DISP_CLEAR   0x01
  #define HD44780_DISP_OFF     0x08
  #define HD44780_DISP_ON      0x0C
  #define HD44780_CURSOR_ON    0x0E
  #define HD44780_CURSOR_BLINK 0x0F
  #define HD44780_RETURN_HOME  0x02 
  #define HD44780_ENTRY_MODE   0x06
  #define HD44780_4BIT_MODE    0x20
  #define HD44780_8BIT_MODE    0x30
  #define HD44780_2_ROWS       0x08
  #define HD44780_FONT_5x10    0x04

  #define HD44780_ROW1_START   0x00
  #define HD44780_ROW2_START   0x40

  // **********************************************
  //                      !!!
  //      MODE DEFINITION - CORRECTLY DEFINED
  //
  // ----------------------------------------------
  //
  //  HD44780_4BIT_MODE - 4 bit mode / 4 data wires 
  //  HD44780_8BIT_MODE - 8 bit mode / 8 data wires    
  //
  // **********************************************
  #define HD44780_MODE         HD44780_4BIT_MODE
  
  // set bit
  #define SETBIT(REG, BIT) { REG |= (1 << BIT); }
  // clear bit
  #define CLRBIT(REG, BIT) { REG &= ~(1 << BIT); }
  // set port / pin if bit is set
  #define SET_IF_BIT_IS_SET(REG, PORT, DATA, BIT) { if((DATA & BIT) > 0) { SETBIT(REG, PORT); } }
  
  /**
   * @desc    LCD init - initialisation routine
   *
   * @param   void
   * @return  void
   */
  void HD44780_Init(void);

  /**
   * @desc    LCD display clear
   *
   * @param   void
   * @return  void
   */
  void HD44780_DisplayClear (void);

  /**
   * @desc    LCD display on
   *
   * @param   void
   * @return  void
   */
  void HD44780_DisplayOn (void);

  /**
   * @desc    LCD cursor on, display on
   *
   * @param   void
   * @return  void
   */
  void HD44780_CursorOn (void);

  /**
   * @desc    LCD cursor blink, cursor on, display on
   *
   * @param   void
   * @return  void
   */
  void HD44780_CursorBlink (void);

  /**
   * @desc    LCD draw char
   *
   * @param  char
   * @return  void
   */
  void HD44780_DrawChar (char character);

  /**
   * @desc    LCD draw string
   *
   * @param   char *
   * @return  void
   */
  void HD44780_DrawString (char *str);


  /**
   * @desc    Check Busy Flag (BF) in 8 bit mode
   *
   * @param   void
   * @return  void
   */
  void HD44780_CheckBFin8bitMode (void);

  /**
   * @desc    Check Busy Flag (BF) in 4 bit mode
   *
   * @param   void
   * @return  void
   */
  void HD44780_CheckBFin4bitMode (void);

  /**
   * @desc    LCD send instruction
   *
   * @param   unsigned char
   * @return  void
   */
  void HD44780_SendInstruction (unsigned short int);

  /**
   * @desc    LCD send data
   *
   * @param   unsigned char
   * @return  void
   */
  void HD44780_SendData (unsigned short int);

  /**
   * @desc    LCD send 4bits instruction in 4 bit mode
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_Send4bitsIn4bitMode (unsigned short int);

  /**
   * @desc    LCD send 8bits instruction in 4 bit mode
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_Send8bitsIn4bitMode (unsigned short int);

  /**
   * @desc    LCD send 8bits instruction in 8 bit mode
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_Send8bitsIn8bitMode (unsigned short int);

  /**
   * @desc    LCD send upper nibble
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_SetUppNibble (unsigned short int);

  /**
   * @desc    LCD send lower nibble
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_SetLowNibble (unsigned short int);

  /**
   * @desc    LCD pulse E
   *
   * @param   void
   * @return  void
   */
  void HD44780_PulseE (void);

  /**
   * @desc    Set PORT DB4 to DB7
   *
   * @param   void
   * @return  void
   */
  void HD44780_SetPORT_DB4to7 (void);

  /**
   * @desc    Set DDR DB4 to DB7
   *
   * @param   void
   * @return  void
   */
  void HD44780_SetDDR_DB4to7 (void);

  /**
   * @desc    Clear DDR DB4 to DB7
   *
   * @param   void
   * @return  void
   */
  void HD44780_ClearDDR_DB4to7 (void);

#endif
