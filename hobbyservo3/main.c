

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/adc.h>

volatile int position1;

/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_TIM3);
    rcc_periph_clock_enable(RCC_GPIOC);
    rcc_periph_clock_enable(RCC_GPIOA);
    rcc_periph_clock_enable(RCC_ADC1);
    
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);

}

void adc_setup(void) {  
  
  gpio_set_mode(GPIOA, GPIO_MODE_INPUT, GPIO_CNF_INPUT_ANALOG, GPIO0);

  /* Make sure the ADC doesn't run during config. */
  adc_power_off(ADC1);
  /* We configure everything for one single conversion. */
  adc_disable_scan_mode(ADC1);
  adc_set_single_conversion_mode(ADC1);
  adc_disable_external_trigger_regular(ADC1);
  adc_set_right_aligned(ADC1);
  adc_set_sample_time_on_all_channels(ADC1, ADC_SMPR_SMP_28DOT5CYC);
  adc_power_on(ADC1);

  
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
    
    timer_clear_flag(TIM2, TIM_SR_UIF);   
    gpio_clear(GPIOC, GPIO13);
}

uint16_t read_adc(uint8_t channel) {
  uint8_t channel_array[16];  
  channel_array[0] = channel;
  
  adc_set_regular_sequence(ADC1, 1, channel_array);
  adc_start_conversion_direct(ADC1);
  while (!adc_eoc(ADC1));  
  return adc_read_regular(ADC1);
     
}


int main(void){

    unsigned int i;
  
    clock_setup();
    gpio_setup();
    adc_setup();
    timer2_init();
    timer3_init();
    
   
    position1 = 36000;
    
    while (1) {
    
      for (i = 0; i < 4000; i++);   
       
      position1 = 12000 + (read_adc(0) * 8);          
    
    }
    
    return 0;
}
