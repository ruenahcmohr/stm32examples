#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/adc.h>
#include "HighComms.h"

static uint16_t peakVal[4];


static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 rcc_periph_clock_enable(RCC_USART1);
 rcc_periph_clock_enable(RCC_AFIO); // enable alternate fn clock for moved serial port.
 rcc_periph_clock_enable(RCC_ADC1);
 rcc_periph_clock_enable(RCC_ADC2);
}


static void usart_setup(void) {
/* Setup GPIO pin GPIO_USART1_TX. RX=PA10  TX=PA9 */
// gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);/* Setup GPIO pin GPIO_USART1_TX. */
 AFIO_MAPR |= AFIO_MAPR_USART1_REMAP; /* Enable USART1 pin software remapping. */ 
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO6); /* Setup GPIO pin GPIO_USART1_TX. */
 
 usart_set_baudrate(USART1, 38400); /* Setup UART parameters. */
 usart_set_databits(USART1, 8);
 usart_set_stopbits(USART1, USART_STOPBITS_1);
 usart_set_mode(USART1, USART_MODE_TX_RX);
 usart_set_parity(USART1, USART_PARITY_NONE);
 usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
 usart_enable(USART1); /* Finally enable the USART. */
 
}

static void adc_setup(void) {  
  
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0 | GPIO1 | GPIO2 | GPIO3 );

  /* Make sure the ADC doesn't run during config. */
  adc_power_on(                         ADC1);
  adc_calibrate(                        ADC1);  
  
  adc_power_off(                        ADC1);
  /* We configure everything for one single conversion. */
  adc_disable_scan_mode(                ADC1);
  
  adc_set_single_conversion_mode(       ADC1);
  
  adc_disable_external_trigger_regular( ADC1);
  adc_set_right_aligned(                ADC1);
  adc_set_sample_time_on_all_channels(  ADC1, ADC_SMPR_SMP_28DOT5CYC);
  adc_power_on(                         ADC1);
  
}


void readPeaks( void ) {
  
  uint8_t i;
 // adc_set_regular_sequence(    ADC1, 1,  (uint8_t[]){0, 1, 2, 3} );
  
  for (i = 0; i != 4; i++) {
  //  adc_set_regular_sequence(    ADC1, 1,  (uint8_t[]){i} );
    adc_set_regular_sequence(    ADC1, 1,  &i );
    adc_start_conversion_direct( ADC1 );
    while (!adc_eoc(ADC1));  
    peakVal[i] = adc_read_regular(ADC1);
  }
     
}


uint16_t read_adc(uint8_t channel) {
  uint8_t channel_array[16];  
  channel_array[0] = channel;
  
  adc_set_regular_sequence(    ADC1, 1, channel_array);
  adc_start_conversion_direct( ADC1 );
  
  while (!adc_eoc(ADC1));  
  return adc_read_regular(ADC1);
     
}

int main(void) {
 uint16_t j;

 clock_setup();
 usart_setup();
 adc_setup();
 
 USART_printstring("serialADC2\r\n");  
 
 while (1) {
 
   readPeaks();
   
   printSignedNumber16(((peakVal[0]*10000)/4096)-5000);
   USART_printstring(", ");
   printSignedNumber16(((peakVal[1]*10000)/4096)-5000);
   USART_printstring(", ");
   printSignedNumber16(((peakVal[2]*10000)/4096)-5000);
   USART_printstring(", ");
   printSignedNumber16(((peakVal[3]*10000)/4096)-5000);
   USART_printstring("\r\n");  
   
   
   for (j = 0; j < 32768; j++) { /* Wait a bit. */
      __asm__("nop");
   }
   
 }

 
 return 0;
}
