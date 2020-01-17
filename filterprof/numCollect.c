#include "numCollect.h"

volatile double value;
unsigned char   valueReady;

unsigned char collectDig(char c) {

  static unsigned char crspos;
  static unsigned char buffer[8];
  static unsigned char mode;

  if (0) {
  
  } else if (c == 0) { // reset
        
    buffer[0] = 90; buffer[1] = 90; buffer[2] = 90; buffer[3] = 90;
    buffer[4] = 90; buffer[5] = 90; buffer[6] = 90; buffer[7] = 90;
    crspos     = 0;
    valueReady = 0;
    mode       = 0;
  
  } else if (c == '\r') {
  
     processString(buffer);// process value  
      
  } else {
  
     if (0) {  
     } else if (c == '.') {     
        if ( mode == 0) {
          buffer[crspos] = 10;  
          crspos = limit(crspos+1, 0, 7);
          mode = 1;
        } else {
          return -1;
        }
      } else if (c == 'k') {     
        if ( mode == 0) {
          buffer[crspos] = 11;  
          crspos = limit(crspos+1, 0, 7);
          mode = 1;
        } else {
          return -1;
        }       
      } else if ((c == 'm')|(c == 'M')){     
        if ( mode == 0) {
          buffer[crspos] = 12;  
          crspos = limit(crspos+1, 0, 7);
          mode = 1;
        } else {
          return -1;
        }         
     } else if (c == BS) {
         crspos = limit(crspos-1, 0, 7);
         if (buffer[crspos] >= 10) mode = 0;
         buffer[crspos] = 90;              
     } else {     
       if ( ( c >= '0' ) & ( c <= '9')) { 
         buffer[crspos] = c - '0';
         crspos = limit(crspos+1, 0, 7);
       } else {
         return -1;
       }    
     } 
             
  }
  
  updateDisplay(buffer);
  
   return 0;
  
}


// process value into a string.
void processString(unsigned char *in) {

  unsigned char crspos   = 0;
  double        tvalue   = 0;
  unsigned int  mode     = 0; // whole numbers
  double        postMult = 1;

  while((crspos < 8) & (in[crspos] != 90)) {
    if (0) {
    } else if (in[crspos] == 10) {         // decimal place
      mode = 10;
      
    } else if (in[crspos] == 11) {         // 'k'
      postMult = 1000; mode = 10;
      
    } else if (in[crspos] == 12) {         // m / M
      postMult = 1000000; mode = 10;   
                  
    } else if (mode == 0 ){                // normal next digit
      tvalue *= 10;
      tvalue += in[crspos];  
        
    } else {                               // next fractional digit.
      tvalue += (double)in[crspos]/mode;
      mode *= 10;
      
    }
    crspos++;
  }
  
  value = tvalue * postMult;
  valueReady = 1;

}


















