/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 23:22:13
 */
#include "pwm.h"

TIM_TypeDef *pwmtim;

void pwm_init(timer &tim, u8 enable_channel, u32 freq, u8 remap){
    pwmtim = tim.tim;
    tim.init();
	tim.set_frequency(freq);
    for(u8 i = 1; i < 5; i++){
        if(enable_channel & (1 << (i - 1))){
            tim.set_channel_mode((TIM_CHx)i, pwm, remap);
	        tim.enable_channel((TIM_CHx)i);
        }
    }
	tim.enable();
}

void pwm_set_freq(u32 f){
    u32 arr = 72000000 / f;
    u32 psc = 1;
    while(arr > 65536){
        arr >>= 1;
        psc <<= 1;
    }
    pwmtim->ARR = arr - 1;
    pwmtim->PSC = psc - 1; 
}

void pwm1out(float duty){
	pwmtim->CCR1 = (u32)(duty * pwmtim->ARR);
}
void pwm2out(float duty){
	pwmtim->CCR2 = (u32)(duty * pwmtim->ARR);
}
void pwm3out(float duty){
	pwmtim->CCR3 = (u32)(duty * pwmtim->ARR);
}
void pwm4out(float duty){
	pwmtim->CCR4 = (u32)(duty * pwmtim->ARR);
}
