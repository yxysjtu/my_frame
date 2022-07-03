#ifndef I2C_H
#define I2C_H

#include "basic.h"

typedef enum{
    WRITE = 0,
    READ = 1
} rw_t;

class I2C{
private:
    pin scl, sda;

    u8 wait_ack();
    void ack();
    void nack();

public:
    void init(pin scl, pin sda);

    u8 start(u8 addr, rw_t rw); //0:write, 1:read

    u8 write(u8 data);

    u8 write(u8 *data, u32 len);

    u8 read(u8 reply_ack=1);

    void end();

};

#endif
