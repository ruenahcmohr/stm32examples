
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include "stmcommon.h"



    #define SDA              GPIO7
    #define SCL              GPIO6
  
    #define I2C_RCC          RCC_GPIOA

    #define SCL_GPIO         GPIOA
    #define SCL_BRR          GPIOA_BRR
    #define SCL_BSRR         GPIOA_BSRR
    
    #define SDA_GPIO         GPIOA
    #define SDA_BRR          GPIOA_BRR
    #define SDA_BSRR         GPIOA_BSRR    
 

    #define ClkLow()    SCL_BRR  = SCL
    #define ClkHigh()   SCL_BSRR = SCL
    #define IsClkLow()  (gpio_get(SCL_GPIO, SCL) ? 0 : 1)

    #define DatLow()    SDA_BRR  = SDA
    #define DatHigh()   SDA_BSRR = SDA
    #define IsDatHigh() (gpio_get(SDA_GPIO, SDA) ? 1 : 0)

    #define I2C_READ    1
    #define I2C_WRITE   0


void Delay(uint32_t d) ;



static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 
 
}




int main(void) {

  

  clock_setup();
 



  rcc_periph_clock_enable(I2C_RCC);

  gpio_set_mode(SCL_GPIO, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, SCL);
  gpio_set_mode(SDA_GPIO, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_OPENDRAIN, SDA);

  DatHigh();
  ClkHigh();     
  
  
  while(1){    
  
    DatLow();
    ClkHigh(); 
  //  Delay(40000);
    
    DatHigh();
    ClkLow();
  //  Delay(40000);
  
  }




  return 0;
}





void Delay(uint32_t d) {
  uint32_t i;
  for (i = 0; i <d; i++)    /* Wait a bit. */
    NOP();
}































