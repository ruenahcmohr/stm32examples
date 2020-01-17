#ifndef __numCollect_h
  #define __numCollect_h

  #include "stmcommon.h"

  #define BS 0x08
 // #define limit(v,l,h)        (((v) > (h)) ? (h) : ((v) < (l)) ? (l) : (v))

  unsigned char collectDig(char c) ;
  void processString(unsigned char *in) ;
  extern void updateDisplay(unsigned char *in);

  extern volatile double value;
  extern unsigned char   valueReady;
  
#endif
