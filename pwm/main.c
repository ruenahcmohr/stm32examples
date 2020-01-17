/******************************

output 25% duty on PA8


******************************/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

#define NOP()  __asm__("nop")

/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM1);
    
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOA);
  //  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL , GPIO8); // PA8
}


// duration timer, high res
void timer1_init(void){
    
    
  //  timer_reset(TIM1);
    timer_set_mode(                 TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);  
    timer_set_prescaler(            TIM1, 10);    
    timer_set_period(               TIM1, 65535);
    timer_continuous_mode(          TIM1 ); // seems to be default
    timer_enable_break_main_output( TIM1 );    
    timer_enable_oc_output(         TIM1, TIM_OC1);
    timer_set_oc_mode(              TIM1, TIM_OC1, TIM_OCM_PWM1);    
    timer_enable_counter(           TIM1 );    
    timer_set_oc_value(             TIM1, TIM_OC1, 16384);                       
       
      
  //  nvic_enable_irq(NVIC_TIM1_IRQ);
  //  timer_enable_irq(TIM1, TIM_DIER_UIE);
}

void tim1_isr(void) { 

  //  timer_set_counter(TIM2, 32768);
  //  timer_enable_counter(TIM2);
    
    timer_clear_flag(TIM1, TIM_SR_UIF);
    
  //  gpio_toggle(GPIOC, GPIO13);
  //  gpio_clear(GPIOC, GPIO13);
}


int main(void){

  unsigned long d;

    clock_setup();
    gpio_setup();
    timer1_init();
   
    
    while (1) {
    //  gpio_toggle(GPIOA, GPIO8);
    //  for ( d = 0; d < 60000; d++)  NOP();    
    }
    
    return 0;
}


/// ---------------------- junk -----------------------------








// interval timer, low res.
void timer3_init(void){

    rcc_periph_clock_enable(RCC_TIM3);
    
    timer_set_prescaler(TIM3, 24); // 36Mhz / 12 / 60000 = 50Hz
    timer_set_period(TIM3, 60000);
    
    nvic_enable_irq(NVIC_TIM3_IRQ);
    timer_enable_irq(TIM3, TIM_DIER_UIE);
    
    timer_enable_counter(TIM3);

}

void tim3_isr(void) { 
    timer_clear_flag(TIM3, TIM_SR_UIF);
    
    gpio_set(GPIOC, GPIO13);
    timer_set_counter(TIM2, 6000);  // ms *4000   6000 = 1.5ms
    timer_enable_counter(TIM2);
}




