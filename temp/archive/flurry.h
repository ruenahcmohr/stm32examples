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

    // ******----- Adapt these SCA and SCL port and pin definition to your target !!
    //
    #define SDA             1           // SDA Port D, Pin 4   
    #define SCL             0           // SCL Port D, Pin 5

    #define SDA_PORT        PORTB       // SDA Port D
    #define SDA_DDR         DDRB
    #define SDA_PIN         PINB

    #define SCL_PORT        PORTB       // SCL Port D         
    #define SCL_DDR         DDRB   
    #define SCL_PIN         PINB


    // ******----------------------------------------------------------------------


    #define ClkLow()    ClearBit( SCL, SCL_PORT )
    #define ClkIp()     ClearBit( SCL, SCL_DDR  )
    #define ClkOp()     SetBit(   SCL, SCL_DDR  )
    #define IsClkLow()  IsHigh(   SCL, SCL_PIN  )

    #define DatLow()    ClearBit( SDA, SDA_PORT )
    #define DatIp()     ClearBit( SDA, SDA_DDR  )
    #define DatOp()     SetBit(   SDA, SDA_DDR  )
    #define IsDatHigh() IsHigh(   SDA, SDA_PIN  )


    #define I2C_READ    1
    #define I2C_WRITE   0

     void          i2c_init      ( void);
     void          i2c_stop      ( void);
     unsigned char i2c_start     ( unsigned char addr);
     unsigned char i2c_rep_start ( unsigned char addr);
     void          i2c_start_wait( unsigned char addr);
     unsigned char i2c_write     ( unsigned char data);
     unsigned char i2c_readAck   ( void);
     unsigned char i2c_readNak   ( void);
     unsigned char i2c_read      ( unsigned char ack);

    #define i2c_readAck()  i2c_read(0)
    #define i2c_readNak()  i2c_read(1)        

  //  #define i2c_read(ack)  (ack) ? i2c_readAck() : i2c_readNak(); 

#endif
