

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include "stmcommon.h"
#include "LS595.h"


/*


it takes 7 us to write 8 bits to the 595


*/

volatile unsigned char flag;

/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13); // GPIO_MODE_OUTPUT_2_MHZ
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO14);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_10_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO15);
}


void timer2_init(void){

    
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_DOWN);
    timer_set_prescaler(TIM2, 900); // 4Khz
    timer_set_period(TIM2, 10000);
    timer_one_shot_mode(TIM2);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
}

void tim2_isr(void) { 
    unsigned char j;
    
    timer_set_counter(TIM2, 4); //SHMOWe ZOW! 99 for a period of 100Hz
    timer_enable_counter(TIM2);
    
    timer_clear_flag(TIM2, TIM_SR_UIF);
    
   // gpio_toggle(GPIOC, GPIO13);
   
    LS595StrobeL();
    LS595Send(~flag);
    LS595StrobeH();
    flag = 0;
   
   
}


int main(void){

    unsigned long d;
 
    clock_setup();
    gpio_setup();
    timer2_init();
    
    timer_set_counter(TIM2, 10);
    timer_enable_counter(TIM2);
    
    while (1) {
      for (d = 800000 ; d ; d--) NOP();
      flag = 1;        
    }
    
    
    
    return 0;
}
