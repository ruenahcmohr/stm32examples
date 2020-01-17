// *************************************************************************
// Title	:    I2C (Single) Master Implementation
// Author:    Peter Fleury <pfleury@gmx.ch>
//            based on Atmel Appl. Note AVR300
//            Ported to C by rue_mohr, sorry, its not as good at the asm version.
// File:      $Id: i2cmaster.S,v 1.13 2015/09/16 11:21:00 peter Exp $
// Software:  AVR-GCC 4.x
// Target:    any device.
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
#include "i2c_master.h"


/*************************************************************************
; delay half period
; For I2C in normal mode (100kHz), use T/2 > 5us
; For I2C in fast mode (400kHz),   use T/2 > 1.25us
;*************************************************************************
*/


/*
void i2c_delay_T2() {
  
  uint8_t x;  
  //for (x = 15; x != 0; x--) {
  for (x = 5; x != 0; x--) {
    NOP();
  }
 
}
*/


void i2c_init(void) {

 // rcc_periph_clock_enable(I2C_RCC); // it looks like if this is called twice, the port stops working.

  gpio_set_mode(SCL_GPIO, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, SCL);
  gpio_set_mode(SDA_GPIO, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, SDA);

  DatHigh();
  ClkHigh();       

}


void i2c_start(uint8_t addr){

  DatLow();
  i2c_delay_T2();
  i2c_write(addr);      

}        
	


uint8_t i2c_rep_start(uint8_t addr) {
        
  DatLow();        i2c_delay_T2();
  DatHigh();       i2c_delay_T2();
  ClkHigh();       i2c_delay_T2();
  DatLow();        i2c_delay_T2();

  return i2c_write(addr);          

}


void i2c_start_wait(uint8_t addr){

  DatLow();      	i2c_delay_T2();

  while(i2c_write(addr) == 0) {      // device not busy -> done
    i2c_stop();	                   // terminate write operation
    DatLow();     i2c_delay_T2();
  }

}
	


void i2c_stop(void) {
        
  ClkLow();
  DatLow();   i2c_delay_T2();
  ClkHigh();  i2c_delay_T2();
  DatHigh();  i2c_delay_T2();
        
}



// return 0 = write successful, 1 = write failed
uint8_t i2c_write( uint8_t data ){         
        
  uint8_t mask = 0x80;

  do {
    ClkLow();

    if (data & mask) {
      DatHigh();          
    } else {
      DatLow();
    }
    i2c_delay_T2();
    ClkHigh();
    i2c_delay_T2();

    mask >>= 1;
  } while(mask);

  ClkLow();  
  DatHigh();    
  i2c_delay_T2();       
  ClkHigh();

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



uint8_t i2c_read(uint8_t ack) {       
        
  uint8_t mask = 0x80;  
  uint8_t data = 0;
  
  do {
    ClkLow(); 
    DatHigh();     i2c_delay_T2();   
    ClkHigh();     i2c_delay_T2();  

    while(IsClkLow());

    if (IsDatHigh()) {
      data |= mask;
    } 

    mask >>= 1;
  } while(mask);
  
  ClkLow(); 
  
   if (ack) {
     DatLow(); 
   } else {
     DatHigh(); 
   }
  i2c_delay_T2();  
  ClkHigh(); 
  
  while(IsClkLow());
  i2c_delay_T2();  
  
  return data;
  
}


