/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-13 18:16:37
 */
#include "servo.h"

void Servo::init(timer &tim, TIM_CHx ch, u8 remap){
    switch (ch){
        case TIM_CH1: ccr = &(tim.tim->CCR1); break;  
        case TIM_CH2: ccr = &(tim.tim->CCR2); break;  
        case TIM_CH3: ccr = &(tim.tim->CCR3); break;  
        case TIM_CH4: ccr = &(tim.tim->CCR4); break;  
        default: break;
    }
    tim.init();
	tim.set_frequency(44999, 31);
    tim.set_channel_mode(ch, pwm, remap);
    *ccr = (uint16_t)map(90, 0, 180, 1125, 5625);
	tim.enable_channel(ch);
	tim.enable();
}

//0~180 -> 0.5~2.5
void Servo::set_pos(float deg){ //arr 45000, psc 32, 1ms=2250
    *ccr = (uint16_t)map(deg, 0, 180, 1125, 5625);
    this->deg = deg;
}
float Servo::read(){
    return deg;
}
