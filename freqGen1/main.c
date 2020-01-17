/*

9600 N81 
  RX on PB11
  TX on PB10

4017 #1
  clk PA0 (pin 14)
  rst PA4 (pin 15)
  
4017 #2
  clk PA1 
  rst PA5

4017 #3
  clk PA2
  rst PA6
  
4017 #4
  clk PA3 
  rst PA7




*/

#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <libopencm3/stm32/usart.h>

#define HaveSerData(usart) ((USART_SR(usart) & USART_SR_RXNE) != 0)
#define NOP()  __asm__("nop")



/* Set STM32 to 72 MHz. */
void clock_setup(void){

    rcc_clock_setup_in_hse_8mhz_out_72mhz();     
  //  rcc_clock_setup_in_hse_8mhz_out_40mhz();
    rcc_periph_clock_enable(RCC_TIM2);
    rcc_periph_clock_enable(RCC_USART3);
}

void gpio_setup(void){

    rcc_periph_clock_enable(RCC_GPIOA);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO4);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO5);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO6);
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO7);
    
    rcc_periph_clock_enable(RCC_GPIOC);
    gpio_set_mode(GPIOC, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_PUSHPULL, GPIO13);
}

static void usartInit(void) {

 /* Setup GPIO pin GPIO_USART1_TX. */
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART3_TX);
 gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_FLOAT, GPIO_USART3_RX);
  
 /* Setup UART parameters. */
 usart_set_baudrate(USART3, 9600);
 usart_set_databits(USART3, 8);
 usart_set_stopbits(USART3, USART_STOPBITS_1); 
 usart_set_parity(USART3, USART_PARITY_NONE);
 usart_set_flow_control(USART3, USART_FLOWCONTROL_NONE);
 
 usart_set_mode(USART3, USART_MODE_TX_RX);
  
 usart_enable(USART3); /* Finally enable the USART. */
 
}


void usart_send_string( char* string) {
   while(*string) {
     usart_send_blocking(USART3, *string);
     string++;
   }
}

/* duration timer, high res

  channel 1 is on PA0 (clk)
  channel 2 is on PA1 (clk)
  channel 3 is on PA2 (clk)
  channel 4 is on PA3 (clk)

*/
void timer2_init(void){
    
    timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);
    timer_set_period(TIM2, 65535);  
    timer_set_prescaler(TIM2, 1);
    timer_disable_preload(TIM2);  
    
    timer_continuous_mode(TIM2);   
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO0); 
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO1);   
    gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_2_MHZ,  GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO2);  
    
    timer_enable_oc_output(TIM2, TIM_OC1);
    timer_disable_oc_clear(TIM2, TIM_OC1);            //     as per OC1CE page 328
    timer_disable_oc_preload(TIM2, TIM_OC1);          //     as per OC1PE page 329
    timer_set_oc_slow_mode(TIM2, TIM_OC1);            //     as per OC1FE page 329
    timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_ACTIVE); //     as per OC1M  page 329
    timer_set_oc_value(TIM2, TIM_OC1, 30000);                                   
    timer_set_oc_polarity_high(TIM2, TIM_OC1);        //     as per CC1P page 333
    timer_enable_oc_output(TIM2, TIM_OC1);            //     as per CC1E page 333            
    
  
    
                
    timer_enable_counter(TIM2);
    
  //  nvic_enable_irq(NVIC_TIM2_IRQ);
  //  timer_enable_irq(TIM2, TIM_DIER_CC1IE); // compare 1 interrupt

}


void pwm_setup(void) {
 /* Configure GPIOs: OUT=PA7 */
 gpio_set_mode(GPIOA, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_TIM2_CH1_ETR );
 timer_reset(TIM2);
 
 timer_set_period(TIM2, 6);
 timer_set_prescaler(TIM2, 0);
   
 timer_set_mode(TIM2, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP); // not centre mode. count up.
 timer_set_oc_mode(TIM2, TIM_OC1, TIM_OCM_PWM1);
 
 timer_set_oc_value(TIM2, TIM_OC1, 3);
 
 timer_enable_oc_preload(TIM2, TIM_OC1);  
 timer_set_oc_polarity_high(TIM2, TIM_OC1); 
 timer_enable_oc_output(TIM2, TIM_OC1);
  
 timer_enable_preload(TIM2);
 timer_continuous_mode(TIM2);
     
 timer_enable_counter(TIM2);
}


int main(void){

    clock_setup();
    gpio_setup();
    pwm_setup();
    
 //   timer2_init();  
 //   usartInit();
  
    
    
    while (1){
   //   if (HaveSerData(USART3)) ;                  
    }
    
    
    return 0;
}

















