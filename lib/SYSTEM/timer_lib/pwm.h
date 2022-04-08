/*
 * @Description: support only 1 timer
 * @Author: yu
 * @LastEditTime: 2022-04-04 23:21:31
 */
#ifndef PWM_H
#define PWM_H

#include "timer.h"

//enable_channel: each digit stands for 1 channel. for example: 0010 -- channel2
void pwm_init(timer &tim, u8 enable_channel, u32 freq=500, u8 remap=0);

void pwm_set_freq(u32 f);

void pwm1out(float duty);
void pwm2out(float duty);
void pwm3out(float duty);
void pwm4out(float duty);

#endif
