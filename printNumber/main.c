#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "HighComms.h"


static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 rcc_periph_clock_enable(RCC_USART1);/* Enable clocks for USART1. */
}


static void usart_setup(void) {

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
 uint16_t i,j;

 clock_setup();
 usart_setup();
 
 i = 0;
 while (1) {
  
   printNumber16(i);
   USART_Transmit('\r');
   
   i++;
   
   for (j = 0; j < 65535; j++) { /* Wait a bit. */
      __asm__("nop");
   }
   
 }

 
 return 0;
}
