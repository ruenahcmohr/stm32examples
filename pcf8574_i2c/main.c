
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "i2c_master.h"

#define Dev8574a 0x70
#define Dev8574  0x40

void writeDS(unsigned char data) ;
void Delay(uint32_t d) ;


int main(void) {

  rcc_clock_setup_in_hse_16mhz_out_72mhz();
  

  i2c_init();

  Delay(4);

  while(1){    

    writeDS(0x00);
    Delay(400000);
    writeDS(0xFF);
    Delay(400000);

  }


  while (1); /* Halt. */

  return 0;
}



void writeDS(unsigned char data) {
  i2c_start(Dev8574a + I2C_WRITE);      
  i2c_write(data); // make sure osc is going
  i2c_stop();
}

void Delay(uint32_t d) {
  uint32_t i;
  for (i = 0; i <d; i++)    /* Wait a bit. */
    NOP();
}
