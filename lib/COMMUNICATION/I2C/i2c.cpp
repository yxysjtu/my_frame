#include "i2c.h"
#include "led.h"

#define SCL GPIOout(scl)
#define SDA GPIOout(sda)
#define SDA_READ GPIOin(sda)

#define SDA_OUT pinMode(sda, OUTPUT_OPEN_DRAIN, HIGH)
#define SDA_IN pinMode(sda, INPUT)

#define DELAY delay(8)

u8 I2C::wait_ack() {
    u8 t = 0;

	SDA = 1;
    SDA_IN;
	DELAY;
    SCL = 1;
    DELAY;

    while(SDA_READ != 0){
        if(++t > 250){
            end();
            return 0;
        }
    }
    SCL = 0;
    return 1;
}

void I2C::ack() {
    SDA_OUT;
    SCL = 0;
    SDA = 0;
    DELAY;
    SCL = 1;
    DELAY;
    SCL = 0;
}

void I2C::nack() {
    SDA_OUT;
    SCL = 0;
    SDA = 1;
    DELAY;
    SCL = 1;
    DELAY;
    SCL = 0;
}

void I2C::init(pin scl, pin sda) {
	this->scl = scl;
	this->sda = sda;
    pinMode(scl, OUTPUT_OPEN_DRAIN, HIGH);
    pinMode(sda, OUTPUT_OPEN_DRAIN, HIGH);
}

u8 I2C::start(u8 addr, rw_t rw) {
    SDA_OUT;
    SDA = 1;
    SCL = 1;
    DELAY;
    SDA = 0;
    DELAY;
    SCL = 0;

    return write((addr << 1) + rw);
	//return write(0xa1);
}

u8 I2C::write(u8 data) {
    SDA_OUT;
    SCL = 0;
    for(u8 i = 0; i < 8; i++){
        SDA = data >> 7;
        data <<= 1;
        DELAY;
        SCL = 1;
        DELAY;
        SCL = 0;
        DELAY;
    }
    return wait_ack();
}

u8 I2C::read(u8 reply_ack) {
    u8 result = 0;
    SDA_IN;
    for(u8 i = 0; i < 8; i++){
        SCL = 0;
        DELAY;
        SCL = 1;
        result <<= 1;
        if(SDA_READ) result++;
        DELAY;
    }
    if(reply_ack) ack();
    else nack();

    return result;
}

void I2C::end() {
    SDA_OUT;
    SCL = 0;
    SDA = 0;
    DELAY;
    SCL = 1;
    SDA = 1;
    DELAY;
}

u8 I2C::write(u8 *data, u32 len) {
    for(u32 i = 0; i < len; i++){
        if(!write(data[i])) return 0;
    }
    return 1;
}
