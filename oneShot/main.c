

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>


/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}


void timer2_init(void){
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_DOWN);
    timer_set_prescaler(TIM2, 36000);
    timer_set_period(TIM2, 10000);
    timer_one_shot_mode(TIM2);
}



int main(void){

    clock_setup();
    gpio_setup();
    timer2_init();
    
    while (1) {    
       
        timer_set_counter(TIM2, 1000);
        timer_enable_counter(TIM2);
    
        while (!(TIM2_SR & (TIM_SR_UIF))) ;    // polling for update interrupt flag
        TIM2_SR &= ~TIM_SR_UIF;                // clear update interrupt flag
        
        gpio_toggle(GPIOC, GPIO13);
    }
    
    return 0;
}
