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

void usart_send_string(uint32_t usart, char* string) ;

static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 /* Enable clocks for GPIO port B (for GPIO_USART3_TX) and USART3. */
 rcc_periph_clock_enable(RCC_USART1);
 rcc_periph_clock_enable(RCC_USART2);
 rcc_periph_clock_enable(RCC_USART3);
}

static void usart_setup(void) {

 /* Setup GPIO pin GPIO_USART1_TX. */
 gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,
 GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
 /* Setup UART parameters. */
 usart_set_baudrate(USART1, 38400);
 usart_set_databits(USART1, 8);
 usart_set_stopbits(USART1, USART_STOPBITS_1);
 usart_set_mode(USART1, USART_MODE_TX_RX);
 usart_set_parity(USART1, USART_PARITY_NONE);
 usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
 /* Finally enable the USART. */
 usart_enable(USART1);
 
}


static void gpio_setup(void) {
 /* Set GPIO12 (in GPIO port C) to 'output push-pull'. */
 gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,
 GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
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
 usart_setup();

 /* Blink the LED (PC12) on the board with every transmitted byte. */
 while (1) {
  gpio_toggle(GPIOC, GPIO13); /* LED on/off */
  // usart_send_blocking(USART1, c + '0'); /* USART1: Send byte. */
  usart_send_string(USART1, "Hello world\r\n");
 }

 for (i = 0; i < 800000; i++) { /* Wait a bit. */
  __asm__("nop");
 }
 
 return 0;
}
