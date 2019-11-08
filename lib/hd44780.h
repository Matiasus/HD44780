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

  // define clock
  #if defined(__AVR_ATMEGA8__)
    #define _FCPU 8000000
  #elif defined(__AVR_ATMEGA16__)
    #define _FCPU 16000000
  #endif

  // define register for HD44780 communication
  #if defined(__AVR_ATMEGA8__) || defined(__AVR_ATMEGA16__)

    // E port
    #ifndef HD44780_DDR_E
      #define HD44780_DDR_E   DDRC
    #endif  
    #ifndef HD44780_PORT_E
      #define HD44780_PORT_E  PORTC
    #endif
    #ifndef HD44780_E
      #define HD44780_E       PORT0
    #endif      
    
    // RS port
    #ifndef HD44780_DDR_RS
      #define HD44780_DDR_RS  DDRC
    #endif
    #ifndef HD44780_PORT_RS
      #define HD44780_PORT_RS PORTC
    #endif    
    #ifndef HD44780_RS
      #define HD44780_RS      PORT1
    #endif
    
    // DATA port 
    #ifndef HD44780_DDR_DB
      #define HD44780_DDR_DB   DDRB
    #endif
    #ifndef HD44780_PORT_DB
      #define HD44780_PORT_DB  PORTB
    #endif    
    #ifndef HD44780_DB7    
      #define HD44780_DB7 PORT7 // LCD PORT DB7
    #endif
    #ifndef HD44780_DB6
      #define HD44780_DB6 PORT6 // LCD PORT DB6
    #endif
    #ifndef HD44780_DB5    
      #define HD44780_DB5 PORT5 // LCD PORT DB5
    #endif
    #ifndef HD44780_DB4    
      #define HD44780_DB4 PORT4 // LCD PORT DB4
    #endif
    #ifndef HD44780_DB3    
      #define HD44780_DB3 PORT3 // LCD PORT DB3
    #endif
    #ifndef HD44780_DB2    
      #define HD44780_DB2 PORT2 // LCD PORT DB2
    #endif
    #ifndef HD44780_DB1    
      #define HD44780_DB1 PORT1 // LCD PORT DB1
    #endif
    #ifndef HD44780_DB0    
      #define HD44780_DB0 PORT0 // LCD PORT DB0
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

  #define HD44780_4BIT_INIT_SEQ     0x03
  #define HD44780_8BIT_INIT_SEQ     0x30
  
  #define HD44780_DISP_CLEAR   0x01
  #define HD44780_DISP_OFF     0x08
  #define HD44780_DISP_ON      0x0C
  #define HD44780_CURSOR_ON    0x0E
  #define HD44780_CURSOR_BLINK 0x0F  
  #define HD44780_ENTRY_MODE   0x06
  #define HD44780_4BIT_MODE    0x02
  
  // set bit
  #define SETBIT(REG, BIT) { REG |= (1 << BIT); }
  // clear bit
  #define CLRBIT(REG, BIT) { REG &= (1 << BIT); }
  // set port / pin if bit is set
  #define SET_IF_BIT_IN_DATA_SET(PORT, PIN, BYTE, BIT) { if(BYTE & BIT) {SETBIT(PORT, PIN)} };
  
  /** @enum Type of operation read / write */
  typedef enum {
    e4BIT = 4;
    e8BIT = 0
  } EMode;
  
  /**
   * @desc    LCD init - initialisation routine
   *
   * @param   EMode
   * @return  void
   */
  void HD44780_Init(EMode);

  /**
   * @desc    LCD send instruction
   *
   * @param   unsigned short int
   * @param   EMode
   * @return  void
   */
  void HD44780_SendInstruction(unsigned short int, EMode);

  /**
   * @desc    LCD out data
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_SendData(unsigned short int);

  /**
   * @desc    LCD out upper and lower nibbles
   *
   * @param   unsigned short int
   * @param   EMode {e4BIT; e8BIT}
   * @return  void
   */
  void HD44780_SendNibbles(unsigned short int, EMode);

  /**
   * @desc    LCD send upper nibble
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_SendUppNibble(unsigned short int);

  /**
   * @desc    LCD send
   *
   * @param   unsigned short int
   * @return  void
   */
  void HD44780_SendLowNibble(unsigned short int);

  /**
   * @desc    LCD pulse E
   *
   * @param   void
   * @return  void
   */
  void HD44780_PulseE(void);

#endif
