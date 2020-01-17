#ifndef __A9833_h
  #define __A9833_h

  #include <stdint.h>
  #include "stmcommon.h"
  #include <libopencm3/stm32/gpio.h>

  // Bit positions, PORTC

  #define A9833SPort              GPIOB_BSRR
  #define A9833CPort              GPIOB_BRR

  #define A9833Strobe             1
  
  #define A9833Data               11
  #define A9833Clock              10
 
  #define A9833StrobeH()    A9833SPort = (1<<A9833Strobe)
  #define A9833StrobeL()    A9833CPort = (1<<A9833Strobe)
  #define A9833DataH()      A9833SPort = (1<<A9833Data)
  #define A9833DataL()      A9833CPort = (1<<A9833Data)
  #define A9833ClockH()     A9833SPort = (1<<A9833Clock)
  #define A9833ClockL()     A9833CPort = (1<<A9833Clock)
  
  #define A9833ClockPulse() A9833ClockL(); NOP();  A9833ClockH()
  
  #define A9833SendOne()    A9833DataH();  NOP();  A9833ClockPulse() 
  #define A9833SendZero()   A9833DataL();  NOP();  A9833ClockPulse() 


  void Ad9833SetFreq ( double f, uint8_t channel ) ;
  void AD9833Init (void) ;
  void AD9833Write16 (uint16_t data)    ;
  void AD9833Delay (uint16_t d)    ;



#endif
