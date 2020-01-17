#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include "HighComms.h"


static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 rcc_periph_clock_enable(RCC_USART1);
 rcc_periph_clock_enable(RCC_ADC1);
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

static void adc_setup(void) {  
  
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);

  /* Make sure the ADC doesn't run during config. */
  adc_power_off(ADC1);
  /* We configure everything for one single conversion. */
  adc_disable_scan_mode(ADC1);
  adc_set_single_conversion_mode(ADC1);
  adc_disable_external_trigger_regular(ADC1);
  adc_set_right_aligned(ADC1);
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);
  adc_power_on(ADC1);

  
}

uint16_t read_adc(uint8_t channel) {
  uint8_t channel_array[16];  
  channel_array[0] = channel;
  
  adc_set_regular_sequence(ADC1, 1, channel_array);
  adc_start_conversion_direct(ADC1);
  while (!adc_eoc(ADC1));  
  return adc_read_regular(ADC1);
     
}

int main(void) {
 uint16_t i,j;

 clock_setup();
 usart_setup();
 adc_setup();
 
 i = 0;
 while (1) {
 
   i = read_adc(0);
   printNumber16(i);
   USART_Transmit('\r');
   
   
   for (j = 0; j < 65535; j++) { /* Wait a bit. */
      __asm__("nop");
   }
   
 }

 
 return 0;
}
