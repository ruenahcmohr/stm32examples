

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>


/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}


void timer2_init(void){

    rcc_periph_clock_enable(RCC_TIM2);

    timer_set_prescaler(TIM2, 1000);
    timer_set_period(TIM2, 24000);
    timer_enable_counter(TIM2);

}



int main(void){

    clock_setup();
    gpio_setup();
    timer2_init();
    
    while (1) {
        while (!(TIM2_SR & (TIM_SR_UIF))) ;    // polling for update interrupt flag
        TIM2_SR &= ~TIM_SR_UIF;                // clear update interrupt flag
        
        gpio_toggle(GPIOC, GPIO13);
    }
    
    return 0;
}
