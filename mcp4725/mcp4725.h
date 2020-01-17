#ifndef __mcp4725_h
  #define __mcp4725_h
  
  #include <stdint.h>
  #include "i2c_master.h"
  
  
  /********************************************
               MCP4725 I2C ADDRESSES              
               
               Code   PartNumber
               ------------------
               AJ--    MCP4725A0T  (my china part says AJVV)
               AP--    MCP4725A1T
               AQ--    MCP4725A2T
               AR--    MCP4725A3T
                             

  #define mcp4725A0_A0_GND (0x60<<1)
  #define mcp4725A0_A0_VDD (0x61<<1)
  #define mcp4725A1_A0_GND (0x62<<1)
  #define mcp4725A1_A0_VDD (0x63<<1)
  #define mcp4725A2_A0_GND (0x64<<1)
  #define mcp4725A2_A0_VDD (0x65<<1)
  #define mcp4725A3_A0_GND (0x66<<1)
  #define mcp4725A3_A0_VDD (0x67<<1)
  
  no, you cant float A0, thanks for asking.
  
    ********************************************/
  
  #define AJ_A0_GND (0x60<<1)
  #define AJ_A0_VDD (0x61<<1)
  #define AP_A0_GND (0x62<<1)
  #define AP_A0_VDD (0x63<<1)
  #define AQ_A0_GND (0x64<<1)
  #define AQ_A0_VDD (0x65<<1)
  #define AR_A0_GND (0x66<<1)
  #define AR_A0_VDD (0x67<<1)
  
  
  
  /*********************************************
            MCP4725  REGISTER BITS                      
  **********************************************/
   // Bits 4,5
   #define mcp4725_fastActive                 (0<<4)
   #define mcp4725_fastSleep1kPD              (1<<4)
   #define mcp4725_fastSleep100kPD            (2<<4)
   #define mcp4725_fastSleep500kPD            (3<<4)

   // Bits 2,1
   #define mcp4725_slowActive                 (0<<1)
   #define mcp4725_slowSleep1kPD              (1<<1)
   #define mcp4725_slowSleep100kPD            (2<<1)
   #define mcp4725_slowSleep500kPD            (3<<1)
   
   // Bits 5,6
   #define mcp4725_slowTargetDACOnly          (2<<5)
   #define mcp4725_slowTargetDACnEPROM        (3<<5)

   
   #define mcp4725_readBusy                   (1<<15)
   


  /*****************************************************
              MCP4725 MACRO FUNCTIONS      
  ****************************************************/


  #define mcp4725IsReady(A)              ((ads1115Read( ADS1115_REG_CONFIG) & ADS1115_ADC_READY ) != 0)
  
  
  
  /*****************************************************
              MCP4725   FUNCTIONS      
  ****************************************************/ 
  

  void     mcp4725WriteFast(   uint8_t addr, uint8_t  config,  uint16_t value );
  void     mcp4725WriteSlow(   uint8_t addr, uint16_t config,  uint16_t value );
  uint16_t mcp4725Read(        uint8_t addr  );
  

  
#endif
