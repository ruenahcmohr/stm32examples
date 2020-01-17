

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>

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
    
    timer_set_prescaler(TIM2, 1000); // 36Mhz / 1000 / 360000 = 1Hz, toggled = 0.5Hz
    timer_set_period(TIM2, 36000);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
    
    timer_enable_counter(TIM2);

}

void tim2_isr(void) { 
    timer_clear_flag(TIM2, TIM_SR_UIF);
    gpio_toggle(GPIOC, GPIO13);
}



int main(void){

    clock_setup();
    gpio_setup();
    timer2_init();
    
    while (1) {
      __asm__("nop");
    }
    
    return 0;
}
