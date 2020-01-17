
#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#include "ads1115.h"
#include "i2c_master.h"
#include "HighComms.h"


void Delay(uint32_t d) ;



static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 rcc_periph_clock_enable(RCC_USART1);
}


static void usart_setup(void) {
/* Setup GPIO pin GPIO_USART1_TX. RX=PA10  TX=PA9 */
 gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);/* Setup GPIO pin GPIO_USART1_TX. */
 
 usart_set_baudrate(USART1, 38400); /* Setup UART parameters. */
 usart_set_databits(USART1, 8);
 usart_set_stopbits(USART1, USART_STOPBITS_1);
 usart_set_mode(USART1, USART_MODE_TX_RX);
 usart_set_parity(USART1, USART_PARITY_NONE);
 usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
 usart_enable(USART1); /* Finally enable the USART. */
 
}



int main(void) {

  clock_setup();
  usart_setup();
  
  i2c_init();
 
  Delay(4);
  
  USART_printstring("Hello\r\n") ;

  while(1){    
  
    printSignedNumber16(ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN0_GND | ADS1115_128SPS | ADS1115_FSR_6144mV ));    
    USART_printstring(", ");    
    
    printSignedNumber16(ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN1_GND | ADS1115_128SPS | ADS1115_FSR_6144mV ));	                  
    USART_printstring(", ");   
     
    printSignedNumber16(ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN2_GND | ADS1115_128SPS | ADS1115_FSR_6144mV ));      
    USART_printstring(", ");  
      
    printSignedNumber16(ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN3_GND | ADS1115_128SPS | ADS1115_FSR_6144mV )); 
    USART_printstring("\r\n");
  }




  return 0;
}





void Delay(uint32_t d) {
  uint32_t i;
  for (i = 0; i <d; i++)    /* Wait a bit. */
    NOP();
}































