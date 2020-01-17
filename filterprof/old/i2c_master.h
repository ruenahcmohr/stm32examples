#ifndef _I2CMASTER_H
  #define _I2CMASTER_H
    /************************************************************************* 
    * Title:    C include file for the I2C master interface 
    *           (i2cmaster.S or twimaster.c)
    * Author:   Peter Fleury <pfleury@gmx.ch>
    * File:     $Id: i2cmaster.h,v 1.12 2015/09/16 09:27:58 peter Exp $
    * Software: AVR-GCC 4.x
    * Target:   any AVR device
    * Usage:    see Doxygen manual
    **************************************************************************/

    #include <stdint.h>
    #include "stmcommon.h"
    #include <libopencm3/stm32/rcc.h>
    #include <libopencm3/stm32/gpio.h>

    // ******----- Adapt these SCA and SCL port and pin definition to your target !!
    //
    

    #define SDA              GPIO7
    #define SCL              GPIO6
  
    #define I2C_RCC          RCC_GPIOA

    #define SCL_GPIO         GPIOA
    #define SCL_BRR          GPIOA_BRR
    #define SCL_BSRR         GPIOA_BSRR
    
    #define SDA_GPIO         GPIOA
    #define SDA_BRR          GPIOA_BRR
    #define SDA_BSRR         GPIOA_BSRR    
 




    // ******----------------------------------------------------------------------

    #define ClkLow()    SCL_BRR  = SCL
    #define ClkHigh()   SCL_BSRR = SCL
    #define IsClkLow()  (gpio_get(SCL_GPIO, SCL) ? 0 : 1)

    #define DatLow()    SDA_BRR  = SDA
    #define DatHigh()   SDA_BSRR = SDA
    #define IsDatHigh() (gpio_get(SDA_GPIO, SDA) ? 1 : 0)

    #define I2C_READ    1
    #define I2C_WRITE   0

    void    i2c_init      ( void);
    void    i2c_stop      ( void);
    void    i2c_start     ( uint8_t addr);
    uint8_t i2c_rep_start ( uint8_t addr);
    void    i2c_start_wait( uint8_t addr);
    uint8_t i2c_write     ( uint8_t data);
    uint8_t i2c_readAck   ( void);
    uint8_t i2c_readNak   ( void);
    uint8_t i2c_read      ( uint8_t ack);

    #define i2c_readAck()  i2c_read(0)
    #define i2c_readNak()  i2c_read(1)        


#endif
