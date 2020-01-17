#ifndef __avr595
  #define __avr595
  
  #include "LS595settings.h"     
  #include "stmcommon.h"

  #include <libopencm3/stm32/gpio.h>

  // Bit positions
  #ifndef LS595SPort
    #error define LS595SPort in LS595settings.h
  #endif
  
  #ifndef LS595CPort
    #error define LS595CPort in LS595settings.h
  #endif
  
  #ifndef LS595Strobe
    #error define LS595Strobe pin in LS595settings.h
  #endif
  
  #ifndef LS595Data
    #error define LS595Data pin  in LS595settings.h
  #endif
  
  #ifndef LS595Clock
    #error define LS595Clock pin in LS595settings.h
  #endif
 
  #define LS595StrobeH()    LS595SPort = (1<<LS595Strobe)
  #define LS595StrobeL()    LS595CPort = (1<<LS595Strobe)
  #define LS595DataH()      LS595SPort = (1<<LS595Data)
  #define LS595DataL()      LS595CPort = (1<<LS595Data)
  #define LS595ClockH()     LS595SPort = (1<<LS595Clock)
  #define LS595ClockL()     LS595CPort = (1<<LS595Clock)
  
  #define LS595ClockPulse() LS595ClockH(); NOP(); LS595ClockL() 
  
  #define LS595SendOne()    LS595DataH(); LS595ClockPulse() 
  #define LS595SendZero()   LS595DataL(); LS595ClockPulse() 
 
void LS595Send (unsigned char bits) ;

#endif
