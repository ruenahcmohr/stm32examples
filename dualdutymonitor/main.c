/******************************

monitor duty on pin A8 0-256
and
                pin A10 0-256


******************************/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>
#include "HighComms.h"

#define NOP()  __asm__("nop")


volatile uint16_t oc;                   // overflow counter extends us to 59 sec 
volatile uint32_t L0, H0, L1, H1;       // buffers of last high and low times
volatile uint32_t L2, H2, L3, H3;       // buffers of last high and low times
volatile uint8_t  NewEdge;




/* Set STM32 to 72 MHz. */
void clock_setup(void){
  rcc_clock_setup_in_hse_8mhz_out_72mhz();  
  rcc_periph_clock_enable(RCC_GPIOA);
  rcc_periph_clock_enable(RCC_GPIOB);
  rcc_periph_clock_enable(RCC_GPIOC);
     
  rcc_periph_clock_enable(RCC_TIM1);
  rcc_periph_clock_enable(RCC_AFIO); // enable alternate fn clock for moved serial port.
  rcc_periph_clock_enable(RCC_USART1);
}

void gpio_setup(void){
  
  
  //  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
 // gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL , GPIO8); // PA8
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT , GPIO8); // PA8
}

void usart_setup(unsigned long baud) {
 
  AFIO_MAPR |= AFIO_MAPR_USART1_REMAP; /* Enable USART1 pin software remapping. */ 
  gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RE_TX); /* Setup GPIO pin GPIO_USART1_TX. */
  
  /* Setup GPIO pin GPIO_USART1_TX. RX=PA10  TX=PA9 */
 // gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);/* Setup GPIO pin GPIO_USART1_TX. */
 
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
  timer_set_period(               TIM1, 65535); // counts between 'update' events (max)
  timer_continuous_mode(          TIM1 ); // seems to be default

/*
  // setup hardware interface (pwm out)
  timer_enable_break_main_output( TIM1 );    
  timer_set_oc_mode(              TIM1, TIM_OC1, TIM_OCM_PWM1); 
  timer_enable_oc_output(         TIM1, TIM_OC1);       
  timer_set_oc_value(             TIM1, TIM_OC1, 16384);                       
*/

  // setup hardware interface ( pulse capture rising edge)
  timer_ic_set_input(             TIM1, TIM_IC1, TIM_IC_IN_TI1 );
  timer_ic_set_filter(            TIM1, TIM_IC1, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC1, TIM_IC_RISING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC1, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC1 );
  
  // setup hardware interface ( pulse capture falling edge)
  timer_ic_set_input(             TIM1, TIM_IC2, TIM_IC_IN_TI1 );
  timer_ic_set_filter(            TIM1, TIM_IC2, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC2, TIM_IC_FALLING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC2, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC2 );  
  
  
  // setup hardware interface ( pulse capture rising edge)
  timer_ic_set_input(             TIM1, TIM_IC3, TIM_IC_IN_TI3 );
  timer_ic_set_filter(            TIM1, TIM_IC3, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC3, TIM_IC_RISING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC3, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC3 );
  
  // setup hardware interface ( pulse capture falling edge)
  timer_ic_set_input(             TIM1, TIM_IC4, TIM_IC_IN_TI3 );
  timer_ic_set_filter(            TIM1, TIM_IC4, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC4, TIM_IC_FALLING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC4, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC4 );  
  
  
  // interrupt for hardware. Couldn't we just use an edge interrupt on the pin? Whatever.
  timer_clear_flag(               TIM1, TIM_SR_CC1IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC1IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC2IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC2IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC3IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC3IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC4IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC4IE);   
  
  
  nvic_enable_irq(                NVIC_TIM1_CC_IRQ);  

  // set up system interrupts                
  nvic_enable_irq(                NVIC_TIM1_UP_IRQ);
  timer_enable_irq(               TIM1, TIM_DIER_UIE);

  // turn on
  timer_enable_counter(           TIM1 );    
}


// rollover counter for intervals counter.
void tim1_up_isr(void) {     
  oc++;
  timer_clear_flag(TIM1, TIM_SR_UIF);
    
}


// timer event
void tim1_cc_isr(void) {

    uint16_t        t, to;
    
    to = oc; 
    
    if (timer_get_flag(TIM1, TIM_SR_CC1IF) ) {   // rising edge
            		       
      //  if (TIM1_CNT < TIM1_CCR1) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR1;  // latched value                
        // --- t:to is now valid        
        
        H0 = H1;
        H1 = to<<16;    
	H1 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC1IF);
        NewEdge++;
    }    
    
    if (timer_get_flag(TIM1, TIM_SR_CC2IF) ) {   // falling edge
            		       
     //   if (TIM1_CNT < TIM1_CCR2) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR2;  // latched value                
        // --- t:to is now valid        
        
        L0 = L1;
        L1 = to<<16;
	L1 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC2IF);
        NewEdge++;
    } 
    
    
    
    
    if (timer_get_flag(TIM1, TIM_SR_CC3IF) ) {   // rising edge
            		       
      //  if (TIM1_CNT < TIM1_CCR3) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR3;  // latched value                
        // --- t:to is now valid        
        
        H2 = H3;
        H3 = to<<16;    
	H3 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC3IF);
        NewEdge++;
    }    
    
    if (timer_get_flag(TIM1, TIM_SR_CC4IF) ) {   // falling edge
            		       
     //   if (TIM1_CNT < TIM1_CCR4) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR4;  // latched value                
        // --- t:to is now valid        
        
        L2 = L3;
        L3 = to<<16;
	L3 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC4IF);
        NewEdge++;
    } 
                       
}



int main(void){
  
  uint32_t     duty, freq, d;
  uint32_t     L0C, H0C, L1C, H1C; 
  uint32_t     L2C, H2C, L3C, H3C;  
  uint64_t     t;
  
    
  clock_setup();
  gpio_setup();
  timer1_init();
  usart_setup(38400);
  oc = 0;
 USART_printstring( "READY: \r\n") ;

  while (1) {

    if (NewEdge) {
      NewEdge = 0;
      L0C = L0;  L1C = L1;  H0C = H0;  H1C = H1; // snapshot (something)
      L2C = L2;  L3C = L3;  H2C = H2;  H3C = H3; // snapshot (something)
      
      // was the last edge high or low
      if ((L0C < L1C) && (H0C < H1C)) { // values valid (and div0 protection)
                    
        if (L1C > H1C) { // last edge was falling   _____H0------L0______H1------L1_____
          d = (L1C-L0C);
          t = (L1C-H1C); t *= 256;          
          duty = (t/d);
                                        
        } else {         // last edge was rising  _____-----L0_____H0------L1______H1------_____
        
	  d = (H1C-H0C);
          t = (L1C-H0C); t *= 256;          
          duty = (t/d);	  
          
        }
	
	freq = (uint32_t)36000000/d;
        
	USART_printstring( "Duty1: 0x") ;
        USART_printHex16(duty); 
        USART_printstring( "/0x0100  ") ;		
      
        USART_printstring( "Freq: ") ;
        printNumber16(freq);
	USART_printstring( "Hz \r\n") ;
      
      }
      
      
      // ----------
      
      
      // was the last edge high or low
      if ((L2C < L3C) && (H2C < H3C)) { // values valid (and div0 protection)
                    
        if (L3C > H3C) { // last edge was falling   _____H0------L0______H1------L1_____
          d = (L3C-L2C);
          t = (L3C-H3C); t *= 256;          
          duty = (t/d);
                                        
        } else {         // last edge was rising  _____-----L0_____H0------L1______H1------_____
        
	  d = (H3C-H2C);
          t = (L3C-H2C); t *= 256;          
          duty = (t/d);	  
          
        }
	
	freq = (uint32_t)36000000/d;
        
	USART_printstring( "Duty2: 0x") ;
        USART_printHex16(duty); 
        USART_printstring( "/0x0100  ") ;		
      
        USART_printstring( "Freq: ") ;
        printNumber16(freq);
	USART_printstring( "Hz \r\n") ;
      
      }
      
      
      
    }
  }

  return 0;
}




















































