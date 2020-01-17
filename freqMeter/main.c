/******************************

output 25% duty on PA8


******************************/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include "HighComms.h"

#define NOP()  __asm__("nop")


volatile uint16_t oc;
volatile uint32_t delta;


/* Set STM32 to 72 MHz. */
void clock_setup(void){
  rcc_clock_setup_in_hse_8mhz_out_72mhz();     
  rcc_periph_clock_enable(RCC_TIM1);
  rcc_periph_clock_enable(RCC_USART1);
}

void gpio_setup(void){
  rcc_periph_clock_enable(RCC_GPIOA);
  //  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
 // gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL , GPIO8); // PA8
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT , GPIO8); // PA8
}

void usart_setup(unsigned long baud) {
 /* Setup GPIO pin GPIO_USART1_TX. RX=PA10  TX=PA9 */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);/* Setup GPIO pin GPIO_USART1_TX. */
  usart_set_baudrate(USART1, baud); /* Setup UART parameters. */
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  usart_enable(USART1); /* Finally enable the USART. */
 
}

// duration timer, high res
void timer1_init(void){

  // set up counter
  timer_set_prescaler(            TIM1, 1); 
  timer_set_mode(                 TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);  
  timer_set_period(               TIM1, 65535); // counts between 'update' events
  timer_continuous_mode(          TIM1 ); // seems to be default

/*
  // setup hardware interface (pwm out)
  timer_enable_break_main_output( TIM1 );    
  timer_set_oc_mode(              TIM1, TIM_OC1, TIM_OCM_PWM1); 
  timer_enable_oc_output(         TIM1, TIM_OC1);       
  timer_set_oc_value(             TIM1, TIM_OC1, 16384);                       
*/

  // setup hardware interface ( pulse capture )
  timer_ic_set_input(             TIM1, TIM_IC1, TIM_IC_IN_TI1 );
  timer_ic_set_filter(            TIM1, TIM_IC1, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC1, TIM_IC_RISING );
  timer_ic_set_prescaler(         TIM1, TIM_IC1, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC1 );
  
  // interrupt for hardware. Couldn't we just use an edge interrupt on the pin? Whatever.
  timer_clear_flag(               TIM1, TIM_SR_CC1IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC1IE);  
  nvic_enable_irq(                NVIC_TIM1_CC_IRQ);

  // set up system interrupts                
  nvic_enable_irq(                NVIC_TIM1_UP_IRQ);
  timer_enable_irq(               TIM1, TIM_DIER_UIE);

  // turn on
  timer_enable_counter(           TIM1 );    
}

void tim1_up_isr(void) {     
  timer_clear_flag(TIM1, TIM_SR_UIF);
  oc++;    
}


void tim1_cc_isr(void) {

    static uint16_t A  = 0;
    static uint16_t AO = 0;
    uint16_t        t, to;
    
    if (timer_get_flag(TIM1, TIM_SR_CC1IF) != 0) {
            		
        to = oc;        
        if (TIM1_CNT < TIM1_CCR1) to--; // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR1;
                        
	delta = t - A + ((to - AO) << 16);
        
	A  = t;  
        AO = to;      // I'd reset oc, but I dont know if/when were ever done with it.
        
        timer_clear_flag(TIM1, TIM_SR_CC1IF);
    }    
    
    
}



int main(void){

  unsigned int dl;
  unsigned int cv;
  uint32_t freq;
  char buff[20];

  clock_setup();
  gpio_setup();
  timer1_init();
  usart_setup(38400);
  oc = 0;

  while (1) {
  //  gpio_toggle(GPIOA, GPIO8);
  //  gpio_clear(GPIOC, GPIO13);
    
    
    //USART_printHex16(oc) ;
    //USART_printHex16(TIM1_CNT) ;
    
    if (delta) {
      freq = (uint32_t)36000000/delta;
      if (delta < 360)  USART_printstring( "*") ;     
    //  sprintf(buff, "%ld", freq);
      printNumber16(freq);
   //   USART_printstring( buff ) ;
      USART_printstring( "Hz \r\n") ;
    }
    
  //  printNumber16((unsigned long)36000000/(unsigned long)delta) ;
  //  USART_printHex16(delta) ;
  
    for ( dl = 0; dl < 60000; dl++)  NOP();    
  }

  return 0;
}




