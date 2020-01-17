/*****
 
 Filter profiler
 
 serial comm
 -------
 rx = B7
 tx = B6

 serial to freq gen
 ------------------
 D = 
 C = 
 
 
 
 
 i2c to ADC
 ----------
SCL = 
SDA = 



 reset line to peak detectors
 ----------------------------
 reset = B12
 
 
 signals from phase detectors
 ----------------------------
 PH0 = 
 PH1 = 
 

 
 *****/


#include <libopencm3/stm32/rcc.h>
#include <libopencm3/stm32/gpio.h>
#include <libopencm3/stm32/usart.h>
#include <libopencm3/stm32/timer.h>
#include <libopencm3/cm3/nvic.h>
#include <stdio.h>

#include "A9833.h"
#include "HighComms.h"
#include "numCollect.h"
#include "i2c_master.h"
#include "ads1115.h"
#include "stmcommon.h"


#define AveragerRun()     GPIOB_BRR  = GPIO12
#define AveragerReset()   GPIOB_BSRR = GPIO12
    
    
void usart_send_string(uint32_t usart, char* string) ;
void takeSample(void);
void Delay(uint32_t d);
void measurePeaks( void ) ;
void measureFreq( void) ;
void waitAveragers(void);


int16_t CH0P, CH0N, CH1P, CH1N;
uint32_t Freq1, Freq2;
//uint16_t Duty1, Duty2;
int16_t  Phaz1, Phaz2;


volatile uint16_t oc;                   // overflow counter extends us to 59 sec 
volatile uint32_t L0, H0, L1, H1;       // buffers of last high and low times
volatile uint32_t L2, H2, L3, H3;       // buffers of last high and low times
volatile uint8_t  NewEdge;


  
  
static void clock_setup(void) {
 rcc_clock_setup_in_hse_8mhz_out_72mhz();
 /* Enable GPIOC clock. */
 rcc_periph_clock_enable(RCC_GPIOA);
 rcc_periph_clock_enable(RCC_GPIOB);
 rcc_periph_clock_enable(RCC_GPIOC);
 
 /* Enable clocks for GPIO port B (for GPIO_USART3_TX) and USART3. */
 rcc_periph_clock_enable(RCC_AFIO); // enable alternate fn clock for moved serial port.
 rcc_periph_clock_enable(RCC_USART1);
 
 rcc_periph_clock_enable(RCC_TIM1);
}


static void usart_setup(void) {
 
 AFIO_MAPR |= AFIO_MAPR_USART1_REMAP; /* Enable USART1 pin software remapping. */ 
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_50_MHZ, GPIO_CNF_OUTPUT_ALTFN_PUSHPULL, GPIO_USART1_RE_TX); /* Setup GPIO pin GPIO_USART1_TX. */ 
 
 /* Setup UART parameters. */
 usart_set_baudrate(     USART1, 38400);
 usart_set_databits(     USART1, 8);
 usart_set_stopbits(     USART1, USART_STOPBITS_1);
 usart_set_mode(         USART1, USART_MODE_TX_RX);
 usart_set_parity(       USART1, USART_PARITY_NONE);
 usart_set_flow_control( USART1, USART_FLOWCONTROL_NONE);
 /* Finally enable the USART. */
 usart_enable(USART1);
 
}


static void gpio_setup(void) {

 // gpio pins for sine wave gen. (A9833)
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, (GPIO10 | GPIO11 | GPIO1) );
 
 // gpio pins for averager reset
 gpio_set_mode(GPIOB, GPIO_MODE_OUTPUT_10_MHZ, GPIO_CNF_OUTPUT_PUSHPULL, (GPIO12) );
 
}



// duration timer, high res
void timer1_init(void){

  // set up counter
  timer_set_prescaler(            TIM1, 1); 
  timer_set_mode(                 TIM1, TIM_CR1_CKD_CK_INT, TIM_CR1_CMS_EDGE, TIM_CR1_DIR_UP);  
  timer_set_period(               TIM1, 65535); // counts between 'update' events (max)
  timer_continuous_mode(          TIM1 ); // seems to be default

/*
  // setup hardware interface (pwm out)
  timer_enable_break_main_output( TIM1 );    
  timer_set_oc_mode(              TIM1, TIM_OC1, TIM_OCM_PWM1); 
  timer_enable_oc_output(         TIM1, TIM_OC1);       
  timer_set_oc_value(             TIM1, TIM_OC1, 16384);                       
*/

  // setup hardware interface ( pulse capture rising edge)
  timer_ic_set_input(             TIM1, TIM_IC1, TIM_IC_IN_TI1 );
  timer_ic_set_filter(            TIM1, TIM_IC1, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC1, TIM_IC_RISING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC1, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC1 );
  
  // setup hardware interface ( pulse capture falling edge)
  timer_ic_set_input(             TIM1, TIM_IC2, TIM_IC_IN_TI1 );
  timer_ic_set_filter(            TIM1, TIM_IC2, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC2, TIM_IC_FALLING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC2, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC2 );  
  
  
  // setup hardware interface ( pulse capture rising edge)
  timer_ic_set_input(             TIM1, TIM_IC3, TIM_IC_IN_TI3 );
  timer_ic_set_filter(            TIM1, TIM_IC3, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC3, TIM_IC_RISING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC3, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC3 );
  
  // setup hardware interface ( pulse capture falling edge)
  timer_ic_set_input(             TIM1, TIM_IC4, TIM_IC_IN_TI3 );
  timer_ic_set_filter(            TIM1, TIM_IC4, TIM_IC_OFF);
  timer_ic_set_polarity(          TIM1, TIM_IC4, TIM_IC_FALLING ); //TIM_IC_RISING
  timer_ic_set_prescaler(         TIM1, TIM_IC4, TIM_IC_PSC_OFF );
  timer_ic_enable(                TIM1, TIM_IC4 );  
  
  
  // interrupt for hardware. Couldn't we just use an edge interrupt on the pin? Whatever.
  timer_clear_flag(               TIM1, TIM_SR_CC1IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC1IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC2IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC2IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC3IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC3IE);    
  
  timer_clear_flag(               TIM1, TIM_SR_CC4IF);
  timer_enable_irq(               TIM1, TIM_DIER_CC4IE);   
  
  
  nvic_enable_irq(                NVIC_TIM1_CC_IRQ);  

  // set up system interrupts                
  nvic_enable_irq(                NVIC_TIM1_UP_IRQ);
  timer_enable_irq(               TIM1, TIM_DIER_UIE);

  // turn on
  timer_enable_counter(           TIM1 );    
}



// rollover counter for intervals counter.
void tim1_up_isr(void) {     
  oc++;
  timer_clear_flag(TIM1, TIM_SR_UIF);
    
}




// timer event
void tim1_cc_isr(void) {

    uint16_t        t, to;
    
    to = oc; 
    
    if (timer_get_flag(TIM1, TIM_SR_CC1IF) ) {   // rising edge
            		       
      //  if (TIM1_CNT < TIM1_CCR1) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR1;  // latched value                
        // --- t:to is now valid        
        
        H0 = H1;
        H1 = to<<16;    
	H1 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC1IF);
        NewEdge++;
    }    
    
    if (timer_get_flag(TIM1, TIM_SR_CC2IF) ) {   // falling edge
            		       
     //   if (TIM1_CNT < TIM1_CCR2) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR2;  // latched value                
        // --- t:to is now valid        
        
        L0 = L1;
        L1 = to<<16;
	L1 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC2IF);
        NewEdge++;
    }             
    
    if (timer_get_flag(TIM1, TIM_SR_CC3IF) ) {   // rising edge
            		       
      //  if (TIM1_CNT < TIM1_CCR3) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR3;  // latched value                
        // --- t:to is now valid        
        
        H2 = H3;
        H3 = to<<16;    
	H3 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC3IF);
        NewEdge++;
    }    
    
    if (timer_get_flag(TIM1, TIM_SR_CC4IF) ) {   // falling edge
            		       
     //   if (TIM1_CNT < TIM1_CCR4) to--;     // it dunn rolled over on us! (!! interrupt race)
	t  = TIM1_CCR4;  // latched value                
        // --- t:to is now valid        
        
        L2 = L3;
        L3 = to<<16;
	L3 += t;
        
        timer_clear_flag(TIM1, TIM_SR_CC4IF);
        NewEdge++;
    } 
                       
}


/*

  The real numbers we should be using are the midpoints between the edges, 
  The zero crossing system may have a voltage offset,
  but the peak should be between the zero crossing.
  Unless this is a nonlinear filter you evil *******

*/
void getTimings( void ) {

  uint32_t     duty, d;
  int32_t     L0C, H0C, L1C, H1C; // should be unsigned
  int32_t     L2C, H2C, L3C, H3C; // should be unsigned 
  int32_t      D1, D2, D3, DR;  
//  uint64_t     t;

  NewEdge = 0;
  while(!NewEdge);
  
  L0C = L0;  L1C = L1;  H0C = H0;  H1C = H1; // snapshot (something)
  L2C = L2;  L3C = L3;  H2C = H2;  H3C = H3; // snapshot (something)

  // was the last edge high or low
  if ((L0C < L1C) && (H0C < H1C)) { // values valid (and div0 protection)

    if (L1C > H1C) { // last edge was falling   _____H0------L0______H1-------L1_____
      d = (L1C-L0C);
    //  t = (L1C-H1C); t *= 256;          
    //  duty = (t/d);

    } else {         // last edge was rising  _____-----L0_____H0-------L1______H1------_____

      d = (H1C-H0C);
   //   t = (L1C-H0C); t *= 256;          
   //   duty = (t/d);	  

    }           
    
    Freq1 = (uint32_t)360000000/d ;
  //  Duty1 = duty;                      
    
    // rising edge search 
    DR = H0C - H2C;
    D1 = H0C - H3C;
    D2 = H1C - H2C ;
    D3 = H1C - H3C ;
    DR = (ABS(D1) < ABS(DR)) ? D1 : DR;
    DR = (ABS(D2) < ABS(DR)) ? D2 : DR;
    DR = (ABS(D3) < ABS(DR)) ? D3 : DR;
       
    Phaz1 = -(DR*360/d);   
       
  }

  // ----------

  // was the last edge high or low
  if ((L2C < L3C) && (H2C < H3C)) { // values valid (and div0 protection)

    if (L3C > H3C) { // last edge was falling   _____H0------L0______H1-------L1_____
      d = (L3C-L2C);
    //  t = (L3C-H3C); t *= 256;          
    //  duty = (t/d);

    } else {         // last edge was rising  _____-----L0_____H0-------L1______H1------_____

      d = (H3C-H2C);
   //   t = (L3C-H2C); t *= 256;          
   //   duty = (t/d);	  

    }
    Freq2 = (uint32_t)360000000/d ;
 //   Duty2 = duty;
    
    // rising edge search 
    DR = H0C - H2C;
    DR = (ABS(H0C - H3C) < ABS(DR)) ? H0C - H3C : DR;
    DR = (ABS(H1C - H2C) < ABS(DR)) ? H1C - H2C : DR;
    DR = (ABS(H1C - H3C) < ABS(DR)) ? H1C - H3C : DR;

    Phaz2 = -(DR*360/d); 

  }      
}

/*

  Track values, wait for them to settle within +-80 counts over 200ms.

*/


void waitAveragers(void) {

  int16_t oCH0P, oCH0N, oCH1P, oCH1N;
  char buff[64];       
  measurePeaks();
  Delay(2000);
  measurePeaks();
  do {
    oCH0P = CH0P ;              oCH0N = CH0N ;              oCH1P = CH1P ;              oCH1N = CH1N ;
    Delay(1200000); // "200ms"
    measurePeaks();   
    /*
   sprintf(buff, "%d, ",     CH0P - oCH0P );
   USART_printstring(buff);
 
   sprintf(buff, "%d, ",     CH0N - oCH0N);
   USART_printstring(buff);
   
   sprintf(buff, "%d, ",     CH1P - oCH1P);
   USART_printstring(buff);

   sprintf(buff, "%d \r\n",  CH1N - oCH1N);
   USART_printstring(buff);
     */

  // 8 is 1.2mV

  } while ( (!inBoundsI(CH0P, oCH0P-80, oCH0P+80)) | (!inBoundsI(CH0N, oCH0N-80, oCH0N+80)) | (!inBoundsI(CH1P, oCH1P-80, oCH1P+80)) | (!inBoundsI(CH1N, oCH1N-80, oCH1N+80)) );

}





int main(void) {

 // uint8_t i;

  clock_setup();
  gpio_setup();
  
  AveragerReset();
  Delay(10);
  AveragerRun();    // hold averager closed
  
  usart_setup();
  i2c_init();

  AD9833Init();  
    
  timer1_init();
          
  USART_printstring("Ready.\r\n");
  collectDig(0); // reset digit collector


  while(1) {               
        
     if(HAVEDATA) { // we should do this in an interrupt so that delay loop down there won't mess things up.
        collectDig(usart_recv(USART1));    
     }

     if (valueReady != 0) {            
         valueReady = 0;        
	 collectDig(0); // reset collector
         if (value < 24020) {  // limit of the ad9833 w/ /512 sin table
          Ad9833SetFreq ( value, 0 ) ;   

          // reset profiler.
          AveragerReset();
          Delay(4000);       // give the freq time to change while resetting the averager
	  
	  L0 = L1 = L2 = L3 = 65535;  // clear time readings
	  H0 = H1 = H2 = H3 = 1;
	  
          AveragerRun();                    
	  waitAveragers();	  	 
	  
	  getTimings();
	  takeSample();  	  	                                 
           
         } else {
          USART_printstring("Out of range 1 <= V <= 24kHz\r\n");
         }	 
      }          
  }
  return 0;
}



/*********************************************************

   Take sample, input rage is 10V (+-5V) full scale of the 
     ADC is about 26666 and we have it offset 5V
     (there is a /2 scale in there too)

*********************************************************/

void takeSample(void) {

 //CH0P, CH0N, CH1P, CH1N;

   char buff[64];            
                
   measurePeaks();
   
   sprintf(buff, "%2.3fV, ",     ((CH0P*10)/26666.0)-5.08 );
   //sprintf(buff, "%X, ",     CH0P );
   USART_printstring(buff);
 
   sprintf(buff, "%2.3fV, ",     ((CH0N*10)/26666.0)-5.08 );
   //sprintf(buff, "%X, ",     CH0N );   
   USART_printstring(buff);
   
   sprintf(buff, "%2.3fV, ",     ((CH1P*10)/26666.0)-5.08 );
   //sprintf(buff, "%X, ",     CH1P );
   USART_printstring(buff);

   sprintf(buff, "%2.3fV, ",  ((CH1N*10)/26666.0)-5.08 );
   //sprintf(buff, "%X\r\n",     CH1N );
   USART_printstring(buff);

   sprintf(buff, "%0.1fHz, ",  Freq1/10.0);
   USART_printstring(buff);
   
   sprintf(buff, "%0.1fHz, ",  Freq2/10.0);
   USART_printstring(buff);
   
   sprintf(buff, "%d deg, ", Phaz1);
   USART_printstring(buff);
   
   sprintf(buff, "%d deg \r\n", Phaz2);
   USART_printstring(buff);


//   printNumber16(Freq1);
//   USART_printstring("  \r\n");
   
   

}

void measurePeaks( void ) {
 
 // there is a huge signed/unsigned mess that I'v successfully balanced out and am now ignoring.
 
 CH0P = ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN0_GND | ADS1115_128SPS | ADS1115_FSR_6144mV );
 CH0N = ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN1_GND | ADS1115_128SPS | ADS1115_FSR_6144mV );
 CH1P = ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN2_GND | ADS1115_128SPS | ADS1115_FSR_6144mV ); 
 CH1N = ads1115BlockedReadSingle( ADS1115_ADDR_GND, ADS1115_MUX_AIN3_GND | ADS1115_128SPS | ADS1115_FSR_6144mV );
  
}




void updateDisplay(unsigned char *in){
   
  unsigned int i;
   
  USART_printstring("\r         \r");

  for ( i = 0; i < 8; i++) {
    if (0) {
    } else if (in[i] == 90) {
       break;
    } else if (in[i] == 10) {
       USART_Transmit ('.');
    } else if (in[i] == 11) {
       USART_Transmit ('k');     
    } else if (in[i] == 12) {
       USART_Transmit ('M');     
    } else {    
       USART_Transmit(in[i]+'0');
    }
  }

}

void Delay(uint32_t d) {
  uint32_t i;
  for (i = 0; i <d; i++)    /* Wait a bit. */
    NOP();
}


