/**************************************

   ADS1115 library 
    an "OMG WHY DO _I_ HAVE TO BE THE ONE WRITE THIS PROPERLY" 
    library by Rue Mohr


***************************************/


#include "ads1115.h"


/*************************** 

    write to ADC register
    
****************************/
void ads1115WriteReg(uint8_t addr, uint8_t reg, uint16_t data) {

  i2c_start(addr | I2C_WRITE);
  i2c_write(reg);
  i2c_write(data >> 8);
  i2c_write(data & 0xFF);
  i2c_stop();
  
}

/**********************************

     read register from ADC
     
***********************************/
uint16_t ads1115ReadReg(uint8_t addr, uint8_t reg){

  uint16_t data = 0;

  i2c_start(addr | I2C_WRITE);
  i2c_write(reg);
  i2c_stop();
  
  i2c_rep_start(addr | I2C_READ);
  data  = i2c_read(1) << 8;
  data |= i2c_read(0);
  i2c_stop();
  
  return data;
}




/*******************************************
  start ADC into continious converstion mode
  
  call this with what you want to do, 
  specifically, channel config | sample rate | amp gain.
  for example:
    ads1115StartContinious( ADS1115_ADDR_GND, 
                            ADS1115_MUX_AIN0_GND |
                            ADS1115_128SPS       |
                            ADS1115_FSR_6144mV   
                          );
  
********************************************/
void ads1115StartContinious(uint8_t addr, uint16_t config){
    
  config                            |=      
     ADS1115_MODE_CONTINUOUS        |   // single conversion mode
     
     ADS1115_COMP_DIS               |   // comparitor disable  -- your comparitor bug is comming from this line of code.
 //  ADS1115_COMP_LAT_NONLATCHING   |   // comparitor non-latching
 //  ADS1115_COMP_POL_ACTIVELOW     |   // comparitor active low
 //  ADS1115_COMP_MODE_TRADITIONAL  |   // comparitor mode single-normal                 
                                    0;
  
  ads1115WriteReg( addr, ADS1115_REG_CONFIG, config);
}




/*******************************************
  start single reading from ADC
  
  call this with what you want to do, 
  specifically, channel config | sample rate | amp gain.
  for example:
    ads1115StartReadSingle( ADS1115_ADDR_GND, 
                            ADS1115_MUX_AIN0_GND |
                            ADS1115_128SPS       |
                            ADS1115_FSR_6144mV   
                          );
  
********************************************/
void ads1115StartReadSingle(uint8_t addr, uint16_t config){
    
  config                            |=      
     ADS1115_MODE_SINGLE            |   // single conversion mode
     ADS1115_OS_SINGLE              |   // do single conversion
     
     ADS1115_COMP_DIS               |   // comparitor disable  -- your comparitor bug is comming from this line of code.
 //  ADS1115_COMP_LAT_NONLATCHING   |   // comparitor non-latching
 //  ADS1115_COMP_POL_ACTIVELOW     |   // comparitor active low
 //  ADS1115_COMP_MODE_TRADITIONAL  |   // comparitor mode single-normal                 
                                    0;
  
  ads1115WriteReg( addr, ADS1115_REG_CONFIG, config);
}





/************************************************

  trigger and wait for single reading from ADC
  params as per ads1115StartReadSingle()
  You can do this same thing in your main loop
  if your polling the adc.
  
*************************************************/
uint16_t ads1115BlockedReadSingle(uint8_t addr, uint16_t config){
  ads1115StartReadSingle( addr, config );      
  while(!ads1115IsReady(addr));  
  return ads1115Result(addr); 
}


