#ifndef __HighComms
  #define __HighComms
  #include <stdint.h>
  #include <libopencm3/stm32/usart.h>

  #define HAVEDATA  ((USART_SR(USART1) & USART_SR_RXNE) != 0)

  void USART_Transmit(uint8_t c) ;
  void USART_printstring( char *data) ;
  void USART_printhex8(uint8_t i) ;
  void USART_printHex16(uint16_t i) ;
  void printDigit(uint8_t n) ;
  void printNumber16(uint16_t n) ;
  void printSignedNumber16(int16_t n) ;
  void printNumber8(uint8_t n) ;
  
#endif
