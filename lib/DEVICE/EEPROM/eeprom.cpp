//
// Created by asus on 2022/7/2.
//

#include "eeprom.h"

EEPROM::EEPROM(eeprom_t type, pin scl, pin sda) : type(type){
    i2c.init(scl, sda);
}

u8 EEPROM::read(u16 addr) {
    u8 result;
    i2c.start(0x50, WRITE);
    if(type > AT24C16) i2c.write(addr >> 8); //MSB8
    i2c.write(addr & 0x00ff); //LSB8
    i2c.start(0x50, READ);
    result = i2c.read(0);
    i2c.end();
    return result;
}

void EEPROM::read(u16 addr, u8 *buf, u32 len) {
    for(u32 i = 0; i < len; i++){
        buf[i] = read(addr + i);
    }
}

void EEPROM::write(u16 addr, u8 data) {
    i2c.start(0x50, WRITE);
    if(type > AT24C16) i2c.write(addr >> 8); //MSB8
    i2c.write(addr & 0x00ff); //LSB8
    i2c.write(data);
    i2c.end();
    delay(10);
}

void EEPROM::write(u16 addr, u8 *data, u32 len) {
    for(u32 i = 0; i < len; i++){
        write(addr + i, data[i]);
    }
}

u8 EEPROM::check(u16 addr) {
    u8 temp = read(addr);
    if(temp == 0x55) return 1;
    write(addr, 0x55);
    temp = read(addr);
    if(temp == 0x55) return 1;
    return 0;
}

