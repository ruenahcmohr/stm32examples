#ifndef __ads1115_
  #define __ads1115_
  
  #include <stdint.h>
  #include "i2c_master.h"
  

  /********************************************
                  ADS1115 I2C ADDRESSES                            
  ********************************************/

  #define ADS1115_ADDR_GND (0x48<<1)
  #define ADS1115_ADDR_VDD (0x49<<1)
  #define ADS1115_ADDR_SDA (0x4A<<1)
  #define ADS1115_ADDR_SCL (0x4B<<1)

  /*********************************************
                 ADS1115 REGISTER ADDRESSES                           
  **********************************************/

  #define ADS1115_REG_CONVERSION  0
  #define ADS1115_REG_CONFIG      1
  #define ADS1115_REG_LO_THRESH   2
  #define ADS1115_REG_HI_THRESH   3


  /*****************************************************
               ADS1115 CONFIG REGISTER BITS                      
  ****************************************************/

  // Bits 0,1    Comparitor averaging
  #define ADS1115_COMP_QUE_1CON          (0<<0)
  #define ADS1115_COMP_QUE_2CON          (1<<0)
  #define ADS1115_COMP_QUE_4CON          (2<<0)
  #define ADS1115_COMP_DIS               (3<<0)

  // Bit 2       Comparitor latch mode
  #define ADS1115_COMP_LAT_NONLATCHING   (0<<2)
  #define ADS1115_COMP_LAT_LATCHING      (1<<2)

  // Bit 3       Comparitor polarity
  #define ADS1115_COMP_ACTIVELOW         (0<<3)
  #define ADS1115_COMP_ACTIVEHIGH        (1<<3)

  // Bit 4       Comparitor type
  #define ADS1115_COMP_MODE_TRADITIONAL  (0<<4)
  #define ADS1115_COMP_MODE_WINDOW       (1<<4)

  // Bits 5,6,7   Sample rate
  #define ADS1115_8SPS                   (0<<5)
  #define ADS1115_16SPS                  (1<<5)
  #define ADS1115_32SPS                  (2<<5)
  #define ADS1115_64SPS                  (3<<5)
  #define ADS1115_128SPS                 (4<<5)
  #define ADS1115_250SPS                 (5<<5)
  #define ADS1115_475SPS                 (6<<5)
  #define ADS1115_860SPS                 (7<<5)

  // Bit 8       Conversion mode
  #define ADS1115_MODE_CONTINUOUS        (0<<8)
  #define ADS1115_MODE_SINGLE            (1<<8) 

  // Bits 9,10,11 Full Scale Range (keep in mind, if your in differential you get +- this)
  #define ADS1115_FSR_6144mV             (0<<9)
  #define ADS1115_FSR_4096mV             (1<<9)
  #define ADS1115_FSR_2048mV             (2<<9)
  #define ADS1115_FSR_1024mV             (3<<9)
  #define ADS1115_FSR_512mV              (4<<9)
  #define ADS1115_FSR_256mV              (5<<9)


  // Bits 12,13,14 Measurement configuration
  #define ADS1115_MUX_AIN0_AIN1          (0<<12)
  #define ADS1115_MUX_AIN0_AIN3          (1<<12)
  #define ADS1115_MUX_AIN1_AIN3          (2<<12)
  #define ADS1115_MUX_AIN2_AIN3          (3<<12)
  #define ADS1115_MUX_AIN0_GND           (4<<12)
  #define ADS1115_MUX_AIN1_GND           (5<<12)
  #define ADS1115_MUX_AIN2_GND           (6<<12)
  #define ADS1115_MUX_AIN3_GND           (7<<12)

  // Bit 15      Conversion start (and ready flag)
  #define ADS1115_OS_SINGLE              (1<<15)
  #define ADS1115_ADC_READY              (1<<15)


  /*****************************************************

              ADS1115 MACRO FUNCTIONS      

  ****************************************************/

  #define ads1115IsReady(A)              ((ads1115ReadReg((A), ADS1115_REG_CONFIG) & ADS1115_ADC_READY ) != 0)
  #define ads1115Result(A)               (ads1115ReadReg( (A), ADS1115_REG_CONVERSION))
  #define ads1115SetCompH(A,V)           ads1115WriteReg((A),  ADS1115_REG_HI_THRESH, (V))
  #define ads1115SetCompL(A,V)           ads1115WriteReg((A),  ADS1115_REG_LO_THRESH, (V))


  void     ads1115WriteReg(          uint8_t addr, uint8_t  reg,    uint16_t data );
  uint16_t ads1115ReadReg(           uint8_t addr, uint8_t  reg                   );
  void     ads1115StartContinious(   uint8_t addr, uint16_t config                );
  void     ads1115StartReadSingle(   uint8_t addr, uint16_t config                );
  uint16_t ads1115BlockedReadSingle( uint8_t addr, uint16_t config                );
  
  
  
  
  
  
#endif   























