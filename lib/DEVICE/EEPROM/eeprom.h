//
// Created by asus on 2022/7/2.
//

#ifndef MYFRAME_EEPROM_H
#define MYFRAME_EEPROM_H

//AT24CXX

#include "basic.h"
#include "i2c.h"

typedef enum{
    AT24C01 = 127,
    AT24C02 = 255,
    AT24C04 = 511,
    AT24C08 = 1023,
    AT24C16 = 2047,
    AT24C32 = 4095,
    AT24C64 = 8191,
    AT24C128 = 16383,
    AT24C256 = 32767
} eeprom_t;

class EEPROM{
private:
    eeprom_t type;
    I2C i2c;
public:
    EEPROM(eeprom_t type, pin scl, pin sda);

    u8 read(u16 addr);
    void read(u16 addr, u8* buf, u32 len);

    void write(u16 addr, u8 data);
    void write(u16 addr, u8* data, u32 len);

    u8 check(u16 addr);
};

#endif //MYFRAME_EEPROM_H
