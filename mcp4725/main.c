
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <math.h>
#include "i2c_master.h"
#include "mcp4725.h"



void Delay(uint32_t d) ;


int main(void) {

  double i; 

  rcc_clock_setup_in_hse_16mhz_out_72mhz();
  rcc_periph_clock_enable(RCC_GPIOA);

  
  
  i2c_init();

  Delay(4);

  while(1){    
    for(i = 0; i < 6.28 ; i+=0.01) {        
      mcp4725WriteFast(  AJ_A0_GND, 0,  2047+2048*sin(i) );
     // Delay(40);    
    }
  }


  while (1); /* Halt. */

  return 0;
}



void Delay(uint32_t d) {
  uint32_t i;
  for (i = 0; i <d; i++)    /* Wait a bit. */
    NOP();
}
