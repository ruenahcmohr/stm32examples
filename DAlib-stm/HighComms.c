
#include "HighComms.h"

void USART_Transmit(uint8_t c) {
  usart_send_blocking(USART1, c);
}

void USART_printstring( char *data) {
    while(*data) {
        USART_Transmit(*data);
        data++;
    }

}

void USART_printhex8(uint8_t i) {
    uint8_t hi,lo;

    hi=i&0xF0;               // High nibble
    hi=hi>>4;
    hi=hi+'0';
    if (hi>'9')
        hi=hi+7;

    lo=(i&0x0F)+'0';         // Low nibble
    if (lo>'9')
        lo=lo+7;

  USART_Transmit( hi );
  USART_Transmit( lo );
}


void USART_printHex16(uint16_t i) {
  USART_printhex8(i >> 8);
  USART_printhex8(i & 0xFF);  
}


void  printDigit(unsigned char n) {
  USART_Transmit( (n & 0x0F) | 0x30 );
}


void printNumber16(uint16_t n) {
  unsigned int d;
    
  d = n/10000;
  printDigit(d);
  n -= d * 10000;
  
  d = n/1000;
  printDigit(d);
  n -= d * 1000;
  
  d = n/100;
  printDigit(d);
  n -= d * 100;
    
  d = n/10;
  printDigit(d);
  n -= d * 10;
  
  printDigit(n);
}


void printSignedNumber16(int16_t n) {

  if (n & 0x8000) {
    USART_Transmit('-') ;
    n =~ n;
    n++;
  }
  
  printNumber16( n);
  
}


void printNumber8(unsigned char n) {
  unsigned char d;
  
  d = n/100;
  printDigit(d);
  n -= d * 100;
    
  d = n/10;
  printDigit(d);
  n -= d * 10;
  
  printDigit(n);
}
