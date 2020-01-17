#ifndef __stmcommon
  #define __stmcommon


  #define NOP()  __asm__("nop")
  


  
  #define SetBit(BIT, PORT)    (PORT |= (1<<BIT))
  #define ClearBit(BIT, PORT)  (PORT &= ~(1<<BIT))
  #define IsHigh(BIT, PORT)    (PORT & (1<<BIT)) != 0
  #define IsLow(BIT, PORT)     (PORT & (1<<BIT)) == 0
  #define NOP()                 __asm__("nop")
  #define ABS(a)                ((a) < 0.0 ? -(a) : (a))
  #define SIGN(x)               (x)==0?0:(x)>0?1:-1
  #define limit(v, l, h)        ((v) > (h)) ? (h) : ((v) < (l)) ? (l) : (v)
  #define inBounds(v, l, h)        ((v) > (h)) ? (0) : ((v) < (l)) ? (0) : (1)
  #define inBoundsI(v, l, h)        ((v) >= (h)) ? (0) : ((v) <= (l)) ? (0) : (1)
  
  #define pulsePin(BIT, PORT)  SetBit(BIT,PORT);NOP();ClearBit(BIT,PORT)
  
  // for linear remapping of number ranges, see arduino map()
  // think of a line, between Il,Ol and Ih,Oh, this solves the y for given x position
  #define RangeRemap(v,Il,Ih,Ol,Oh) (((((v)-(Il))*((Oh)-(Ol)))/((Ih)-(Il)))+(Ol))
  
  #define IsDigit(C)  (((C)>='0') && ((C)<='9'))



#endif
