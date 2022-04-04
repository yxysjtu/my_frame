/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 10:01:25
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
