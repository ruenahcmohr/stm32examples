
#include "mcp4725.h"



  void  mcp4725WriteFast(   uint8_t addr, uint8_t  config,  uint16_t value ) {
    
    value  &= 0x0FFF;
    config &= mcp4725_fastSleep500kPD;
    
    i2c_start(addr | I2C_WRITE);
    i2c_write((value >> 8) | config );
    i2c_write(value & 0xFF);
    i2c_stop();

    return;
  }
  
  
  void  mcp4725WriteSlow(   uint8_t addr, uint16_t config,  uint16_t value ) {
  
    return;
  }
  
  
  uint16_t mcp4725Read(   uint8_t addr  ) {
  
    return 0;
  }
  
  
