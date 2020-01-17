

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>


/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}

void usart_setup(void) {

  rcc_periph_clock_enable(RCC_USART1);
  rcc_periph_clock_enable(RCC_GPIOA);
  
  /* Setup GPIO pin GPIO_USART1_TX. */
  gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_TX);
  
  /* Setup UART parameters. */
  usart_set_baudrate(USART1, 38400);
  usart_set_databits(USART1, 8);
  usart_set_stopbits(USART1, USART_STOPBITS_1);
  usart_set_mode(USART1, USART_MODE_TX_RX);
  usart_set_parity(USART1, USART_PARITY_NONE);
  usart_set_flow_control(USART1, USART_FLOWCONTROL_NONE);
  
  /* Finally enable the USART. */
  usart_enable(USART1);
 
}

void timer2_init(void){

    rcc_periph_clock_enable(RCC_TIM2);
    
    timer_set_prescaler(TIM2, 12); // 36Mhz / 12 / 60000 = 50Hz
    timer_set_period(TIM2, 60000);
    
    nvic_enable_irq(NVIC_TIM2_IRQ);
    timer_enable_irq(TIM2, TIM_DIER_UIE);
    
    timer_enable_counter(TIM2);

}

void tim2_isr(void) { 
    timer_clear_flag(TIM2, TIM_SR_UIF);
    usart_send_blocking(USART1, 'U');
}



int main(void){

    clock_setup();
    gpio_setup();
    usart_setup();
    timer2_init();
    
    usart_send_blocking(USART1, '-');
    
    while (1) {
      __asm__("nop");
    }
    
    return 0;
}
