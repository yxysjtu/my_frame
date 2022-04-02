/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 18:59:09
 */
#include "timer.h"

timer tim1(TIM1);
timer tim2(TIM2);
timer tim3(TIM3);
timer tim4(TIM4);
timer tim5(TIM5);
timer tim6(TIM6);
timer tim7(TIM7);
timer tim8(TIM8);

timer::timer(TIM_TypeDef *tim){
    this->tim = tim;
	update_func = 0;
	trigger_func = 0;
	for(u8 i = 0; i < 4; i++) cc_func[i] = 0;
}

void timer::init(tim_mode mode){
	//enable clock
    if((u32)tim == (u32)TIM1){
        RCC->APB2ENR |= 1 << 11;
    }else if((u32)tim == (u32)TIM8){
        RCC->APB2ENR |= 1 << 13;
    }else{
        u32 timnum = ((u32)tim - TIM2_BASE) / 0x0400;
        RCC->APB1ENR |= 1 << timnum;
    }
    //config
    tim->CR1 |= mode << 3; //OPM
}

void timer::attach_ITR(void (*f)(void), tim_event event, NVIC_priority priority){
    if(event == update) update_func = f;
    else if(event == trigger) trigger_func = f;
    else cc_func[event - 1] = f;

    tim->DIER |= 1 << event; //ITR enable
	
    switch((u32)tim){ //NVIC
        case TIM1_BASE:
            if(event == update) NVIC_init(TIM1_UP_IRQn, priority);
            else if(event == trigger) NVIC_init(TIM1_TRG_COM_IRQn, priority); 
            else NVIC_init(TIM1_CC_IRQn , priority);
            break;
        case TIM2_BASE: NVIC_init(TIM2_IRQn, priority); break;
        case TIM3_BASE: NVIC_init(TIM3_IRQn, priority); break;
        case TIM4_BASE: NVIC_init(TIM4_IRQn, priority); break;
        case TIM5_BASE: NVIC_init(TIM5_IRQn, priority); break;
        case TIM6_BASE: NVIC_init(TIM6_IRQn, priority); break;
        case TIM7_BASE: NVIC_init(TIM7_IRQn, priority); break;
        case TIM8_BASE:
            if(event == update) NVIC_init(TIM8_UP_IRQn, priority);
            else if(event == trigger) NVIC_init(TIM8_TRG_COM_IRQn , priority); 
            else NVIC_init(TIM8_CC_IRQn , priority);
            break;
        default: break;
    }
}
void timer::IRQHandler(void){
    if(tim->SR & (1 << update)){
        if(update_func != 0) update_func();
        tim->SR &= ~(1 << update);
    }else if(tim->SR & (1 << trigger)){
        if(trigger_func != 0) trigger_func();
        tim->SR &= ~(1 << trigger);
    }else{
        for(u8 i = 1; i <= 4; i++){
            if(tim->SR & (1 << i)){
                if(cc_func[i - 1] != 0) cc_func[i - 1]();
                tim->SR &= ~(1 << i);
                break;
            }
        }
    }
}

void timer::enable(){
    tim->CR1 |= 1 << 0; //enable
}
void timer::disable(){
    tim->CR1 &= ~(1 << 0);
}

void timer::set_direction(u8 dir){
    tim->CR1 |= dir << 4;
}
void timer::set_frequency(u32 f){
    u32 arr = 72000000 / f;
    u32 psc = 1;
    while(arr > 65536){
        arr >>= 1;
        psc <<= 1;
    }
    tim->ARR = arr - 1;
    tim->PSC = psc - 1; 
}
void timer::set_frequency(u16 arr, u16 psc){
    tim->ARR = arr;
    tim->PSC = psc;
}
void timer::set_channel_mode(TIM_CHx ch, tim_channel_mode mode, pin p){
    
}
void timer::set_pulsewidth(TIM_CHx ch, float pw){

}

void timer::set_master_out(master_out_source m){

}

void timer::set_slave_in(slave_in_source s){

}

u16 timer::get_cnt(){
    return tim->CNT;
}

#ifdef __cplusplus
extern "C" {
#endif
	
void TIM1_UP_IRQHandler(void){
	tim1.IRQHandler();
}
void TIM1_TRG_COM_IRQHandler(void){
	tim1.IRQHandler();
}
void TIM1_CC_IRQHandler(void){
	tim1.IRQHandler();
}

void TIM2_IRQHandler(void){
	tim2.IRQHandler();
}

void TIM3_IRQHandler(void){
	tim3.IRQHandler();
}

void TIM4_IRQHandler(void){
	tim4.IRQHandler();
}

void TIM5_IRQHandler(void){
	tim5.IRQHandler();
}

void TIM6_IRQHandler(void){
	tim6.IRQHandler();
}

void TIM7_IRQHandler(void){
	tim7.IRQHandler();
}

void TIM8_UP_IRQHandler(void){
	tim8.IRQHandler();
}
void TIM8_TRG_COM_IRQHandler(void){
	tim8.IRQHandler();
}
void TIM8_CC_IRQHandler(void){
	tim8.IRQHandler();
}

#ifdef __cplusplus
}
#endif
