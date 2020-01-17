#include "pin.h"
#include "rcc.h"
#include "delay.h"
#include "serial.h"
#include "i2c.h"
#include "address.h"
#include <stdio.h>

i2cmaster<I2C1> i2c; //this works - tested...
//i2cmaster<I2C2> i2c; //this also works, tested
//This example is for communicating to
// INA219 i2c module for measuring current and voltage.

serial s1;

static void
write_ina219(uint8_t reg, uint16_t val)
{
   i2c.busyWait();

   i2c.start(INA219_ADDRESS, 0); //WRITE
   i2c.write(reg);
   i2c.write((val >> 8) & 0xFF);
   i2c.write(val & 0xFF);
   i2c.stop();
}

/*
   Reading from the Slave
   This is a little more complicated - but not too much more. Before reading data from the slave device, you must tell it which of its internal addresses you want to read. So a read of the slave actually starts off by writing to it. This is the same as when you want to write to it: You send the start sequence, the I2C address of the slave with the R/W bit low (even address) and the internal register number you want to write to. Now you send another start sequence (sometimes called a restart) and the I2C address again - this time with the read bit set. You then read as many data bytes as you wish and terminate the transaction with a stop sequence. So to read the compass bearing as a byte from the CMPS03 module:
   1. Send a start sequence
   2. Send 0xC0 ( I2C address of the CMPS03 with the R/W bit low (even address)
   3. Send 0x01 (Internal address of the bearing register)
   4. Send a start sequence again (repeated start)
   5. Send 0xC1 ( I2C address of the CMPS03 with the R/W bit high (odd address)
   6. Read data byte from CMPS03
   7. Send the stop sequence.
 */
static uint16_t
read_ina219(uint8_t reg)
{
   //wait for busy
   i2c.busyWait();

   uint16_t value = 0;

   i2c.start(INA219_ADDRESS, 0); //master wants to write
   i2c.write(reg); //master writes 1 reg byte to slave
   //With Stop and then Start -> this is called STopStart sequence
   //i2c.stop(); //TODO: we might not need this? check.. Yup INA219 does support RESTART as well
   // as StopSTART sequence
   //without stop, if there is start, then we call it Restart sequence
   // most of the slave i2c devices supports both.

   i2c.start(INA219_ADDRESS, 1);  //master wants to read
   //enable ACK for data recieve except the last bit
   i2c.enableAck();
   value = ((i2c.read() << 8) | i2c.read(true));
   i2c.stop();

   return value;
}

static void
readBusVoltage()
{
   uint16_t value;
   value = read_ina219(INA219_REG_BUSVOLTAGE);
   s1.print("Bus voltage: ");
   //s1.printint(value);
   int16_t val2 = (int16_t)((value >> 3) * 4);
   s1.printint(val2);
   s1.println(" mV");
   time::delay(10);
}

static void
readCurrent()
{
   uint16_t value;
   float valueDec;
   const float ina219_currentDivider_mA = 20;
   const uint16_t ina219_calValue = 8192;

   write_ina219(INA219_REG_CALIBRATION, ina219_calValue);
   time::delay(5);
   value = read_ina219(INA219_REG_CURRENT);
   valueDec = ((float)(value)) /ina219_currentDivider_mA;
     {
        char buf[10];
        sprintf(buf, "%f", valueDec);
        s1.print("Current: ");
        s1.print(buf);
        s1.println(" mA");
     }
}

static void
readShuntVoltage()
{
   uint16_t value;
   float valueDec;

   value = read_ina219(INA219_REG_SHUNTVOLTAGE);
   valueDec = (value) * 0.01;
     {
        s1.print("shunt voltage: ");
        char buf[10];
        sprintf(buf, "%f", valueDec);
        s1.print(buf);
        s1.println(" mV");
     }
}

int main(void)
{
   //set STM32 to 72 MHz
   RCC::defaultClockSetup();

   time::enable(); //enables milliseconds

   RCC::enable(RCC_GPIOC);
   pin p(GPIOC, GPIO13);
   p.setMode(PinMode::OUTPUT_2MHZ, PinConfig::OUTPUT_PUSHPULL);
   p.on();

   s1.begin();

   i2c.begin(50, I2CSpeed::FAST);

   //check if device is on circuit
   if (i2c.start(INA219_ADDRESS, 0))
     {
        s1.println("got the device INA219 ..");
        p.off(); // PB13 led on
        return -1;
     }
   i2c.stop();

   const uint16_t ina219_calValue = 8192;
   uint16_t config = INA219_CONFIG_BVOLTAGERANGE_16V |
      INA219_CONFIG_GAIN_1_40MV |
      INA219_CONFIG_BADCRES_12BIT |
      INA219_CONFIG_SADCRES_12BIT_1S_532US |
      INA219_CONFIG_MODE_SANDBVOLT_CONTINUOUS;

   //The below two write_ina219() set the module into Mode_16V_400mA mode.
   write_ina219(INA219_REG_CALIBRATION, ina219_calValue);
   time::delay(1);
   write_ina219(INA219_REG_CONFIG, config);
   time::delay(1);

   while(1)
     {
        //bus voltage
        readBusVoltage();
        time::delay(1);
        //shunt voltage
        readShuntVoltage();
        time::delay(1);
        //current
        readCurrent();

        p.toggle();

        s1.println("");
        s1.println("");
        s1.println("");

        time::delay(5000);
     }
}
