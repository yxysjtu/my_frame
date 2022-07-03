/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-13 18:16:28
 */
#ifndef SERVO_H
#define SERVO_H

#include "timer.h"

class Servo{
private:
    float deg;
    vu16* ccr;
public:
    void init(timer &tim, TIM_CHx ch, u8 remap=0);

    void set_pos(float deg);
    float angle();
};


#endif
