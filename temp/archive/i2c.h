#ifndef _STM32_I2C_H
#define _STM32_I2C_H

#include <libopencm3/stm32/i2c.h>
#include "pin.h"



enum  I2CSpeed {
  STANDARD, //100Kbps
  FAST // 400Kbps
};



  pin _sda, _scl;

    void     i2c_begin(uint8_t myaddress = 0x32, I2CSpeed mode = I2CSpeed::STANDARD);
    void     i2c_end();

    uint8_t  i2c_start(uint8_t address, uint8_t mode);
    void     i2c_stop();
    void     i2c_write(uint8_t data);
    uint8_t  i2c_read(bool islast = false);

    //this checks for if the i2c lines are busy
    void     i2c_busyWait() ;  
    void     i2c_enableAck() ;
    void     i2c_disableAck() ;
};


#endif
