/*

9600 N81 
  RX on PB11
  TX on PB10

4017 #1
  clk PA0 (pin 14)
  rst PA4 (pin 15)
  
4017 #2
  clk PA1 
  rst PA5

4017 #3
  clk PA2
  rst PA6
  
4017 #4
  clk PA3 
  rst PA7




*/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#define HaveSerData(usart) ((USART_SR(usart) & USART_SR_RXNE) != 0)
#define InBoundsI(v, l, h)        ((v) > (h)) ? (0) : ((v) < (l)) ? (0) : (1)
#define NOP()  __asm__("nop")

#define BASEADDRESS 0

volatile unsigned char idxA, idxB, idxC;

// lookup tables for the interrupts to use, last value is a dummy reset
unsigned char LUTA[] = {0 ,1 ,2 ,3 ,4 ,5 ,6 ,7 ,32};
unsigned char LUTB[] = {8 ,9 ,10,11,12,13,14,15,32};
unsigned char LUTC[] = {16,17,18,19,20,21,22,23,32};
unsigned char LUTD[] = {24,25,26,27,28,29,30,31,32};

// position values
unsigned int positions[33];



/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_USART3);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);
    
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

static void usartInit(void) {

 /* Setup GPIO pin GPIO_USART1_TX. */
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
 gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART3_RX);
  
 /* Setup UART parameters. */
 usart_set_baudrate(USART3, 9600);
 usart_set_databits(USART3, 8);
 usart_set_stopbits(USART3, USART_STOPBITS_1); 
 usart_set_parity(USART3, USART_PARITY_NONE);
 usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
 
 usart_set_mode(USART3, USART_MODE_TX_RX);
  
 usart_enable(USART3); /* Finally enable the USART. */
 
}


void usart_send_string( char* string) {
   while(*string) {
     usart_send_blocking(USART3, *string);
     string++;
   }
}

/* duration timer, high res

  channel 1 is on PA0 (clk)
  channel 2 is on PA1 (clk)
  channel 3 is on PA2 (clk)
  channel 4 is on PA3 (clk)

*/
void timer2_init(void){
    
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_period(TIM2, 65535);  
    timer_set_prescaler(TIM2, 2);
    timer_disable_preload(TIM2);  
    
    timer_continuous_mode(TIM2);   
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0); 
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO1);   
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);  
    
    timer_enable_oc_output(TIM2, TIM_OC1);
    timer_disable_oc_clear(TIM2, TIM_OC1);            //     as per OC1CE page 328
    timer_disable_oc_preload(TIM2, TIM_OC1);          //     as per OC1PE page 329
    timer_set_oc_slow_mode(TIM2, TIM_OC1);            //     as per OC1FE page 329
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    timer_set_oc_value(TIM2, TIM_OC1, 30000);                                   
    timer_set_oc_polarity_high(TIM2, TIM_OC1);        //     as per CC1P page 333
    timer_enable_oc_output(TIM2, TIM_OC1);            //     as per CC1E page 333            
    
    timer_enable_oc_output(TIM2, TIM_OC2);
    timer_disable_oc_clear(TIM2, TIM_OC2);            //     as per OC1CE page 328
    timer_disable_oc_preload(TIM2, TIM_OC2);          //     as per OC1PE page 329
    timer_set_oc_slow_mode(TIM2, TIM_OC2);            //     as per OC1FE page 329
    timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    timer_set_oc_value(TIM2, TIM_OC2, 30000);                                   
    timer_set_oc_polarity_high(TIM2, TIM_OC2);        //     as per CC1P page 333
    timer_enable_oc_output(TIM2, TIM_OC2);            //     as per CC1E page 333        
        
    timer_enable_oc_output(TIM2, TIM_OC3);
    timer_disable_oc_clear(TIM2, TIM_OC3);            //     as per OC1CE page 328
    timer_disable_oc_preload(TIM2, TIM_OC3);          //     as per OC1PE page 329
    timer_set_oc_slow_mode(TIM2, TIM_OC3);            //     as per OC1FE page 329
    timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    timer_set_oc_value(TIM2, TIM_OC3, 30000);                                   
    timer_set_oc_polarity_high(TIM2, TIM_OC3);        //     as per CC1P page 333
    timer_enable_oc_output(TIM2, TIM_OC3);            //     as per CC1E page 333 
    
    // }:]
                
    timer_enable_counter(TIM2);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_CC1IE); // compare 1 interrupt
    timer_enable_irq(TIM2, TIM_DIER_CC2IE); // compare 2 interrupt
    timer_enable_irq(TIM2, TIM_DIER_CC3IE); // compare 3 interrupt
}


void tim2_isr(void) { 

    if (timer_get_flag(TIM2, TIM_SR_CC1IF)) {  // from compare1?
      timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_FORCE_LOW); //     as per OC1M  page 329
      timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_ACTIVE); //     as per OC1M  page 329    
      TIM2_CCR1 += positions[LUTA[idxA]];  
      if (idxA == 8) { /* do 4017 reset */ gpio_set(GPIOA, GPIO4); NOP(); NOP(); gpio_clear(GPIOA, GPIO4); } 
      if (++idxA == 9) idxA = 0;
      timer_clear_flag(TIM2, TIM_SR_CC1IF);
    }
    
    if (timer_get_flag(TIM2, TIM_SR_CC2IF)) {  // from compare2?
      timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_FORCE_LOW); //     as per OC1M  page 329
      timer_set_oc_mode(TIM2, TIM_OC2, TIM_OCM_ACTIVE); //     as per OC1M  page 329    
      TIM2_CCR2 += positions[LUTB[idxB]];   
      if (idxB == 8) { /* do 4017 reset */gpio_set(GPIOA, GPIO5); NOP(); NOP(); gpio_clear(GPIOA, GPIO5); } 
      if (++idxB == 9) idxB = 0;
      timer_clear_flag(TIM2, TIM_SR_CC2IF);
    }
    
    if (timer_get_flag(TIM2, TIM_SR_CC3IF)) {  // from compare3?
      timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_FORCE_LOW); //     as per OC1M  page 329
      timer_set_oc_mode(TIM2, TIM_OC3, TIM_OCM_ACTIVE); //     as per OC1M  page 329    
      TIM2_CCR3 += positions[LUTC[idxC]];  
      if (idxC == 8) { /* do 4017 reset */gpio_set(GPIOA, GPIO6); NOP(); NOP(); gpio_clear(GPIOA, GPIO6); } 
      if (++idxC == 9) idxC = 0;
      timer_clear_flag(TIM2, TIM_SR_CC3IF);
    }
    
    if (timer_get_flag(TIM2, TIM_SR_CC4IF)) {  // from compare4?        
      /*  Nu uh uh.., you have to subscribe to get these channels!
      timer_set_oc_mode(TIM2, TIM_OC4, TIM_OCM_FORCE_LOW); //     as per OC1M  page 329
      timer_set_oc_mode(TIM2, TIM_OC4, TIM_OCM_ACTIVE); //     as per OC1M  page 329    
      TIM2_CCR4 += positions[LUTD[idxD]];   
      idxD = (idxD+1) & 0x07;
      timer_clear_flag(TIM2, TIM_SR_CC4IF);
      */
    }
}



int main(void){

  unsigned char pkt[2];
  unsigned char ptr;
  unsigned int temp;
  unsigned char data;

  unsigned int cache[23];

    clock_setup();
    gpio_setup();
    timer2_init();
    usartInit();
    positions[32] = 1000;  // this is the dummy slot for the reset, we have more than 27us to doddle.
    
    for(ptr = 0; ptr < 32; ptr++)  positions[ptr] = 36100;  // 12100 = ~0.5ms, 24000 = ~1ms
    
    /*
     0  1  2  3  4  5  6  7
     8  9 10 11 12 13 14 15
    16 17 18 19 20 21 22 23
    */ /*
    positions[0] = 10000;
    positions[9] = 10000;
    positions[18] = 10000;
    */
    
    ptr = 0; // forgetting to do this could lead to hours and hours of debugging.
    
    while (1){
      if (HaveSerData(USART3)) {
        //gpio_toggle(GPIOC, GPIO13);
	data = usart_recv(USART3);
	if (0) {
	} else if (ptr == 0) {
	  //gpio_toggle(GPIOC, GPIO13);
          if (data == 255) ptr++;
	} else if (ptr == 1) {
          pkt[0] = data;        
          if (InBoundsI((data & 31), BASEADDRESS, (BASEADDRESS+23)) || (data == 0xFF) ){  // if this is for us, carry on
            ptr++;
          } else {
            ptr = 20;   // otherwise, ignore the rest of the packet
          }
	} else if (ptr == 2) {
          pkt[1] = data;
          ptr++;
	} else if (ptr == 3) {
	    gpio_toggle(GPIOC, GPIO13);
            temp = pkt[1];
	    
            temp <<= 8;
            temp = temp | (data & 0xFF);     	   	
	     
            if (temp > 62000) { temp = 62000;}
            if (pkt[0] < 32) {                           // immediate move      
        	positions[pkt[0] & 31] = temp + 12100; 
            } else if ( pkt[0] < 64) {                   // cache position
        	cache[pkt[0] & 31] = temp + 12100;     
            } else if (pkt[0] == 0xFF) {                 // enguage cached positions
	        for(ptr = 0; ptr < 32; ptr++)  positions[ptr] = cache[ptr]; // dont underestimate me, I was a BASIC coder!	               	    
            }
	    ptr = 0;
	} else if (ptr == 20) {  // ignoring data
          ptr++;
	} else if (ptr == 21) { // ignoring data
          ptr = 0;
	} else {
	  ptr = 0;  // this should never happen unless I forgot to initialize it before the main loop again
	}

      }            
    }
    
    
    return 0;
}

















