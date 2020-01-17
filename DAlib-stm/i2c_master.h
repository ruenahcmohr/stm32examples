#ifndef _I2CMASTER_H
  #define _I2CMASTER_H
    /************************************************************************* 
    * Title:    C include file for the I2C master interface 
    *           (i2cmaster.S or twimaster.c)
    * Author:   Peter Fleury <pfleury@gmx.ch>
    * File:     $Id: i2cmaster.h,v 1.12 2015/09/16 09:27:58 peter Exp $
    * Software: gcc
    * Target:   
    * Usage:    
    **************************************************************************/

  #include <stdint.h>
  #include "stmcommon.h"
  #include "i2c_masterSettings.h"  
  #include <libopencm3/stm32/rcc.h>
  #include <libopencm3/stm32/gpio.h>
    
    
    // ******----- Adapt these SCA and SCL port and pin definition to your target !!
    //  I'v been thru a ringer with gcc for like 3 hours, if you want to split
    //    the data and clock lines to different ports, you can modify the code :)
    
  // Bit positions
  
  #ifndef SDA
    #warning define SDA pin in i2c_masterSettings.h using default GPIO10
    #define SDA              GPIO10
  #endif
  
  #ifndef SCL
    #warning define SCL pin  in i2c_masterSettings.h using default GPIO11
    #define SCL              GPIO11
  #endif
  
  #ifndef i2c_m_port
    #warning define i2c_m_port port in i2c_masterSettings.h using default GPIOB
    #define i2c_m_port        B
  #endif
  
  
   
  // THANKYOU InPhase !!!
  
  #define CONCAT(a, b, c)   CONCAT2(a,b,c)
  #define CONCAT2(a, b, c)   a ## b ## c


 // #define SCL_GPIO      i2c_m_port  
 // #define SDA_GPIO      i2c_m_port 
 // #define I2C_RCC       RCC_GPIOB
//  #define SCL_GPIO      GPIOB 
 // #define SCL_BRR       GPIOB_BRR 
 // #define SCL_BSRR      GPIOB_BSRR 
//  #define SDA_GPIO      GPIOB
//  #define SDA_BRR       GPIOB_BRR 
 // #define SDA_BSRR      GPIOB_BSRR 


  #define SCL_GPIO      CONCAT( GPIO, i2c_m_port, )    
  #define SDA_GPIO      CONCAT( GPIO, i2c_m_port, )       
 

    // ******----------------------------------------------------------------------
    // the concat'd names have to be done all at once, otherwise gcc will start
    // resolving them on us, and it turns into a snot show, and we dont need that.
   
   //  #define ClkLow()    SCL_BRR  = SCL
   // #define ClkHigh()    SCL_BSRR = SCL
   //  #define DatLow()    SDA_BRR  = SDA
   // #define DatHigh()    SDA_BSRR = SDA
  
    #define ClkLow()      CONCAT( GPIO, i2c_m_port , _BRR ) = SCL        
    #define ClkHigh()     CONCAT( GPIO, i2c_m_port , _BSRR ) = SCL    
    #define IsClkLow()   (gpio_get(SCL_GPIO, SCL) ? 0 : 1) 
    #define DatLow()      CONCAT( GPIO, i2c_m_port , _BRR ) = SDA      
    #define DatHigh()     CONCAT( GPIO, i2c_m_port , _BSRR ) = SDA    
    #define IsDatHigh()  (gpio_get(SDA_GPIO, SDA) ? 1 : 0)

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

    #define i2c_delay_T2() NOP()

#endif
