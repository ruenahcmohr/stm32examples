

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/exti.h>
#include <libopencm3/cm3/nvic.h>

static void gpio_setup(void) {

  rcc_periph_clock_enable(RCC_GPIOC);	
  gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
  gpio_set_mode(GPIOC, GPIO_MODE_INPUT,        GPIO_CNF_INPUT_FLOAT,     GPIO14);
        
}

void exti_setup(void) {

  rcc_periph_clock_enable(RCC_AFIO);
  exti_select_source (EXTI14, GPIOC);  
  exti_set_trigger   (EXTI14, EXTI_TRIGGER_BOTH);        
  exti_enable_request(EXTI14);

  nvic_enable_irq(NVIC_EXTI15_10_IRQ);

}


void exti15_10_isr(void) {
  
  if (gpio_get(GPIOC, GPIO14)) {  
    gpio_set(GPIOC, GPIO13);
  } else {
    gpio_clear(GPIOC, GPIO13);
  }
  
  exti_reset_request(EXTI14);
  
}

int main(void) {

  gpio_setup();
  exti_setup();
	
  while (1) ;
  
  return 0;
  
}

