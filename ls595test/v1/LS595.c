#include "LS595.h"

void LS595Send (unsigned char bits) {
  unsigned char temp;  
  for( temp = 0x80; temp != 0; temp >>= 1) {    
    if ( (bits & temp) == 0 ) {
      LS595SendZero();
    } else {
      LS595SendOne();
    }    
  } 
}









