/*****
 *
 * serial hello world
 * rx = A10
 * tx = A9
 *
 *****/


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include "A9833.h"

void usart_send_string(uint32_t usart, char* string) ;

static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 /* Enable clocks for GPIO port B (for GPIO_USART3_TX) and USART3. */
 rcc_periph_clock_enable(RCC_USART1);
}

static void usart_setup(void) {

 /* Setup GPIO pin GPIO_USART1_TX. */
 gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
 /* Setup UART parameters. */
 usart_set_baudrate(     USART1, 38400);
 usart_set_databits(     USART1, 8);
 usart_set_stopbits(     USART1, USART_STOPBITS_1);
 usart_set_mode(         USART1, USART_MODE_TX_RX);
 usart_set_parity(       USART1, USART_PARITY_NONE);
 usart_set_flow_control( USART1, USART_FLOWCONTROL_NONE);
 /* Finally enable the USART. */
 usart_enable(USART1);
 
}


static void gpio_setup(void) {
 /* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, (GPIO10 | GPIO11 | GPIO1) );
 
}

void usart_send_string(uint32_t usart, char* string) {
   while(*string) {
     usart_send_blocking(usart, *string);
     string++;
   }
}

int main(void) {
  unsigned long i;

  clock_setup();
  gpio_setup();
//  usart_setup();

  AD9833Init();      
      
  for (i = 0; i < 800000 ; i++) NOP();

  while(1) {               
    
    for (i = 0; i < 80 ; i++)   NOP();
    
    Ad9833SetFreq ( 2000, 0 );  
    
  }

  return 0;
}
