#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>

#define HaveSerData(usart) ((USART_SR(usart) & USART_SR_RXNE) != 0)


void usart_send_string( char* string) ;

static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 /* Enable clocks for GPIO port B (for GPIO_USART3_TX) and USART3. */
 rcc_periph_clock_enable(RCC_USART3);
}

static void usart_setup(void) {

 /* Setup GPIO pin GPIO_USART1_TX. */
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
 gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART3_RX);
  
 /* Setup UART parameters. */
 usart_set_baudrate(USART3, 9600);
 usart_set_databits(USART3, 8);
 usart_set_stopbits(USART3, USART_STOPBITS_1); 
 usart_set_parity(USART3, USART_PARITY_NONE);
 usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
 
 //usart_enable_rx_interrupt (USART3);
 
 usart_set_mode(USART3, USART_MODE_TX_RX);
  
 usart_enable(USART3); /* Finally enable the USART. */
 
}


static void gpio_setup(void) {
 /* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
 // gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

void usart_send_string( char* string) {
   while(*string) {
     usart_send_blocking(USART3, *string);
     string++;
   }
}




int main(void) {
 unsigned long i;
 unsigned char c;

 clock_setup();
 gpio_setup();
 usart_setup();

 /* Blink the LED (PC12) on the board with every transmitted byte. */
 while (1) {  
  if (HaveSerData(USART3)) {
    c = usart_recv(USART3);
    usart_send_blocking(USART3, '0');
    usart_send_blocking(USART3, 'x');    
    usart_send_blocking(USART3, (((c>>4)&0x0F)>9)?(((c>>4)&0x0F)+'8'):(((c>>4)&0x0F)+'0') );
    usart_send_blocking(USART3, ((c&0x0F)>9)?((c&0x0F)+'8'):((c&0x0F)+'0') );
    usart_send_blocking(USART3, '\r');
    usart_send_blocking(USART3, '\n');
  }  
  __asm__("nop");
 }

 // __asm__("nop");
 
 return 0;
}
