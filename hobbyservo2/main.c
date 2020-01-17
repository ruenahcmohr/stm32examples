

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

volatile int position1;

/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_TIM3);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}


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
    timer_set_counter(TIM2, position1);  // ms *24000   36000 = 1.5ms
    timer_enable_counter(TIM2);
}



// duration timer, high res
void timer2_init(void){
    
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_DOWN);
    timer_set_prescaler(TIM2, 3);
    timer_set_period(TIM2, 65000);
    timer_one_shot_mode(TIM2);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
}

void tim2_isr(void) { 

  //  timer_set_counter(TIM2, 2000);
  //  timer_enable_counter(TIM2);
    
    timer_clear_flag(TIM2, TIM_SR_UIF);
    
  //  gpio_toggle(GPIOC, GPIO13);
    gpio_clear(GPIOC, GPIO13);
}


int main(void){

    unsigned int i;
  
    clock_setup();
    gpio_setup();
    timer2_init();
    timer3_init();
   
    position1 = 36000;
    
    while (1) {
    
      for (i = 0; i < 4000; i++);   
       
      position1++;
      if (position1 > 60000) position1 = 12000;
    
    
    }
    
    return 0;
}
