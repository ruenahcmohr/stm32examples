//#include "i2c.h"
#include "rcc.h"
#include "delay.h"

/*
// 400KHz
if (fastMode)
{
// Datasheet suggests 0x1e.
i2c_set_fast_mode(I2C);
uint32_t clockRatio = rcc_apb1_frequency / 400000;
i2c_set_ccr(I2C, (clockRatio*2+3)/6); //round clockRatio/3
i2c_set_dutycycle(I2C, I2C_CCR_DUTY_DIV2);

 * rise time for 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;
 * Incremented by 1 -> 11.
 uint32_t clocks = 300 / ((2000000000+rcc_apb1_frequency)/(rcc_apb1_frequency*2)) + 1;
 i2c_set_trise(I2C, clocks);
 }
 else
 {
 i2c_set_standard_mode(I2C);
 uint32_t clockRatio = rcc_apb1_frequency / 100000;
 i2c_set_ccr(I2C, clockRatio/2);
 uint32_t clocks = 1000 / ((2000000000+rcc_apb1_frequency)/(rcc_apb1_frequency*2)) + 1;
 i2c_set_trise(I2C, clocks);
 }
 */
 

void i2c_begin(uint8_t ownaddress, I2CSpeed mode) {
   uint8_t freq = I2C_CR2_FREQ_36MHZ;
   
   //stm32f103
   i2c_enable(RCC_I2C1);

   _sda.assign(GPIOB, GPIO_I2C1_SDA); //PB7
   _scl.assign(GPIOB, GPIO_I2C1_SCL); // PB6


   i2c_enable(RCC_GPIOB);
   _sda.setMode(PinMode::OUTPUT_50MHZ, PinConfig::OUTPUT_ALTFOPENDRAIN);
   _scl.setMode(PinMode::OUTPUT_50MHZ, PinConfig::OUTPUT_ALTFOPENDRAIN);
   
   i2c_reset(i2c);
   i2c_peripheral_disable(i2c);
   i2c_set_clock_frequency(i2c, freq);

   //100 Khz mode
   if (mode == STANDARD)    {
        uint32_t clockRatio = rcc_apb1_frequency / 100000;
        uint32_t clocks = 1000 / ((2000000000+rcc_apb1_frequency)/(rcc_apb1_frequency*2)) + 1;

        i2c_set_standard_mode(i2c);
        i2c_set_ccr(i2c, clockRatio/2);
        i2c_set_trise(i2c, clocks);
     }   else { //400Khz mode     
        // rise time for 400kHz => 300ns and 100kHz => 1000ns; 300ns/28ns = 10;  Incremented by 1 -> 11.

        uint32_t clocks = 300 / ((2000000000+rcc_apb1_frequency)/(rcc_apb1_frequency*2)) + 1;
        uint32_t clockRatio = rcc_apb1_frequency / 400000;

        i2c_set_fast_mode(i2c);
        i2c_set_ccr(i2c, (clockRatio*2+3)/6);
        i2c_set_trise(i2c, clocks);
     }

   //we could disable ACK if we want
   //i2c_enable_ack(i2c);
   //this is only required in case of need of communcations to other masters
   i2c_set_own_7bit_slave_address(i2c, ownaddress);
   i2c_peripheral_enable(i2c);
}


void i2c_end(){
   i2c_peripheral_disable(i2c);
}


uint8_t i2c_start(uint8_t address, uint8_t mode) {

   i2c_send_start(i2c);
   //wait for master mode to be selected.

   int timeout = 20000;

   while (!((I2C_SR1(i2c) & I2C_SR1_SB)  & (I2C_SR2(i2c) & (I2C_SR2_MSL | I2C_SR2_BUSY))));

   // both works
   i2c_send_7bit_address(i2c, address, mode);
   // or
   //i2c_send_data((address << 1) | mode);

   //wait for address to be transferred
   timeout = 20000;
   //the below while loop gives 4 clks
   // 72Mhz => 20000*4/72e6 = 0.0011s == 1.1ms timeout

   while (!(I2C_SR1(i2c) & I2C_SR1_ADDR))  {   
        if (timeout > 0)   {
             timeout--;
          } else  {
             //this is a case when slave does not reply after 1.1ms wait
             return 1;
          }
        //TODO: there should be a  timer here for timeout...
        //return 1 if timer is expired
     }

   // Cleaning ADDR condition sequence. 
   uint32_t reg32 = I2C_SR2(i2c);
   (void) reg32; // unused 

   //got the ACK
   return 0;
}


void i2c_stop(){
   //wait for transfer to be completed
   while (!(I2C_SR1(i2c) & (I2C_SR1_BTF | I2C_SR1_TxE)));
   i2c_send_stop(i2c);
}


void i2c_write(uint8_t data){
   //this will hang if you call it for setting address in START, don't do it!
   //while (!(I2C_SR1(i2c) & I2C_SR1_TxE)); //call i2c_send_7bit_address instead
   i2c_send_data(i2c, data);
   while (!(I2C_SR1(i2c) & (I2C_SR1_BTF)));
}


uint8_t i2c_read(bool islast){
   //for last byte, we disable the ack
   // for other bytes,we need ack
   if (islast)
     disableAck();
   
   while((I2C_SR1(i2c) & I2C_SR1_RxNE) == 0);
   return i2c_get_data(i2c);
}

void i2c_busyWait()    {
  while ((I2C_SR2(i2c) & I2C_SR2_BUSY)) {};
}
    
void i2c_enableAck()    {
  i2c_enable_ack(i2c);
}
    
void i2c_disableAck()    {
  i2c_disable_ack(i2c);
}
