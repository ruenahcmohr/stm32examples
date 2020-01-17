// *************************************************************************
// Title	:    I2C (Single) Master Implementation
// Author:    Peter Fleury <pfleury@gmx.ch>
//            based on Atmel Appl. Note AVR300
// File:      $Id: i2cmaster.S,v 1.13 2015/09/16 11:21:00 peter Exp $
// Software:  AVR-GCC 4.x
// Target:    any AVR device
//
// DESCRIPTION
// 	Basic routines for communicating with I2C slave devices. This
//	"single" master implementation is limited to one bus master on the
//	I2C bus. 
//  
//       Based on the Atmel Application Note AVR300, corrected and adapted 
//       to GNU assembler and AVR-GCC C call interface
//       Replaced the incorrect quarter period delays found in AVR300 with 
//       half period delays. 
//
// USAGE
//	These routines can be called from C, refere to file i2cmaster.h.
//       See example test_i2cmaster.c 
// 	Adapt the SCL and SDA port and pin definitions and eventually 
//	the delay routine to your target !
// 	Use 4.7k pull-up resistor on the SDA and SCL pin.
//
// NOTES
//	The I2C routines can be called either from non-interrupt or
//	interrupt routines, not both.
//
// *************************************************************************
#include "flurry.h"


//*************************************************************************
; delay half period
; For I2C in normal mode (100kHz), use T/2 > 5us
; For I2C in fast mode (400kHz),   use T/2 > 1.25us
;*************************************************************************
*/

i2c_delay_T2:        ; 3 cycles
	return;         ; 4 cycle = total 100 cycles = 5.0 microsec with 20 Mhz crystal



void i2c_init(void) {

        DatIp();
        ClkIp();
        DatLow();
        ClkLow();
	return;

}


unsigned char i2c_start(unsigned char addr){

        DatOp();
        i2c_delay_T2();
        i2c_write(addr);      
	return;
}        
	


unsigned char i2c_rep_start(unsigned char addr) {
        
        DatOp();
        i2c_delay_T2();
        DatIp();
        i2c_delay_T2();
        ClkIp();
        i2c_delay_T2();
        DatOp();
        i2c_delay_T2();
        return i2c_write(addr);          

}


void i2c_start_wait(unsigned char addr){

	DatOp();             // force SDA low
	i2c_delay_T2();
        
        while(i2c_write(addr) == 0) { //device not busy -> done
          i2c_stop();	//terminate write operation
          DatOp();             // force SDA low
	  i2c_delay_T2();
        }

	return;

}
	


void i2c_stop(void) {
        
        ClkOp();
        DatOp();
        i2c_delay_T2();
        ClkIp();
        i2c_delay_T2();
        DatIp();
        i2c_delay_T2();
        
	return;
}



// return 0 = write successful, 1 = write failed
unsigned char i2c_write( unsigned char data ){         
        
        uint8_t mask = 0x80;
        
        do {
          ClkOp();
           
          if (data && mask) {
            DatIp();          
          } else {
            DatOp();
          }
          i2c_delay_T2();
          ClkIp();
          i2c_delay_T2();
                                    
          mask >>= 1;
        } while(mask);
            
        ClkOp();  
        DatIp();    
        i2c_delay_T2();       
        ClkIp();
        
        while(IsClkLow());
                
        i2c_delay_T2();
        return IsDatHigh();
        

}




/**************************************************************************
; read one byte from the I2C device, send ack or nak to device
; (ack=1, send ack, request more data from device 
;  ack=0, send nak, read is followed by a stop condition)
;
;*************************************************************************
*/



unsigned char i2c_read(unsigned char ack) {       
        
  uint8_t mask = 0x80;  
  uint8_t value = 0;
  
  do {
    ClkOp(); 
    DatIp(); 
    i2c_delay_T2();   
    ClkIp();
    i2c_delay_T2();  

    while(IsClkLow());

    if (IsDatHigh()) {
      value |= mask;
    } 

    mask >>= 1;
  } while(mask);
  
  ClkOp(); 
  
   if (ack) {
     DatOp(); 
   } else {
     DatIp(); 
   }
  i2c_delay_T2();  
  ClkIp(); 
  
  while(IsClkLow());
  i2c_delay_T2();  
  
  return data;
  
}


