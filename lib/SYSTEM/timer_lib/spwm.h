/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 20:15:54
 */
#ifndef SPWM_H
#define SPWM_H

#include "pwm.h"

u32 spwm_i = 0, spwm_buf_n;
float *spwm_buf;
u8 enable_ch;

void set_duty(){
    if(enable_ch & (1 << 0)) pwm1out(spwm_buf[spwm_i]);
    if(enable_ch & (1 << 1)) pwm2out(spwm_buf[spwm_i]);
    if(enable_ch & (1 << 2)) pwm3out(spwm_buf[spwm_i]);
    if(enable_ch & (1 << 3)) pwm4out(spwm_buf[spwm_i]);
    if(++spwm_i > spwm_buf_n) spwm_i = 0;
}
//enable_channel: each digit stands for 1 channel. for example: 0010 -- channel2
void spwm_init(float* buf, u32 buf_n, timer &pwmtim, timer &cnttim, u8 enable_channel, u32 freq, u8 remap=0){
    pwm_init(pwmtim, enable_channel, 200000, remap); //200khz
    enable_ch = enable_channel;
    spwm_buf = buf;
    spwm_buf_n = buf_n;
    
    cnttim.init();
	cnttim.set_frequency(freq);
	cnttim.attach_ITR(set_duty);
	cnttim.enable();
}

#endif
