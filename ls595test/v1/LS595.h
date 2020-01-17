#ifndef __avr595
  #define __avr595
  
  #include "LS595settings.h"     
  #include "stmcommon.h"
 // #include <avr/io.h>
 #include <libopencm3/stm32/gpio.h>

  // Bit positions
  #ifndef LS595Port
    #error define LS595Port in LS595settings.h
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
 
  #define LS595StrobeH()    SetBit(LS595Strobe,   LS595Port) 
  #define LS595StrobeL()    ClearBit(LS595Strobe, LS595Port) 
  #define LS595DataH()      SetBit(LS595Data,     LS595Port)  
  #define LS595DataL()      ClearBit(LS595Data,   LS595Port) 
  #define LS595ClockH()     SetBit(LS595Clock,    LS595Port) 
  #define LS595ClockL()     ClearBit(LS595Clock,  LS595Port) 
  #define LS595ClockPulse() LS595ClockH(); NOP(); LS595ClockL() 
  #define LS595SendOne()    LS595DataH(); LS595ClockPulse() 
  #define LS595SendZero()   LS595DataL(); LS595ClockPulse() 
 
void LS595Send (unsigned char bits) ;

#endif
