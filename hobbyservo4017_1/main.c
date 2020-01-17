

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>


/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
  //  rcc_periph_clock_enable(RCC_TIM3);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO1);
}



/* duration timer, high res

  channel 1 is on PA0
  channel 2 is on PA1
  channel 3 is on PA2
  channel 4 is on PA3

*/
void timer2_init(void){
    
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_period(TIM2, 65535);  
    timer_set_prescaler(TIM2, 2);
    timer_disable_preload(TIM2);  
    
    timer_continuous_mode(TIM2);   
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0);    
    
    timer_enable_oc_output(TIM2, TIM_OC1);
    timer_disable_oc_clear(TIM2, TIM_OC1);            //     as per OC1CE page 328
    timer_disable_oc_preload(TIM2, TIM_OC1);          //     as per OC1PE page 329
    timer_set_oc_slow_mode(TIM2, TIM_OC1);            //     as per OC1FE page 329
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    timer_set_oc_value(TIM2, TIM_OC1, 30000);   
                                
    timer_set_oc_polarity_high(TIM2, TIM_OC1);        //     as per CC1P page 333
    timer_enable_oc_output(TIM2, TIM_OC1);            //     as per CC1E page 333            
    
    timer_enable_counter(TIM2);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
   // timer_enable_irq(TIM2, TIM_DIER_UIE); // update interrupt
    timer_enable_irq(TIM2, TIM_DIER_CC1IE); // compare 1 interrupt
}


void tim2_isr(void) { 

  //  timer_set_counter(TIM2, 2000);
  //  timer_enable_counter(TIM2);
  
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_FORCE_LOW); //     as per OC1M  page 329
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    
    TIM2_CCR1 += 5000; // example reload of time.
    
   // timer_clear_flag(TIM2, TIM_SR_UIF);
     timer_clear_flag(TIM2, TIM_SR_CC1IF);
    
   // gpio_toggle(GPIOA, GPIO1);
  //  gpio_clear(GPIOC, GPIO13);
}


int main(void){

    clock_setup();
    gpio_setup();
    timer2_init();
   
    
    while (1);
    
    
    return 0;
}

/*************** JUNK ********************




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
































*/
