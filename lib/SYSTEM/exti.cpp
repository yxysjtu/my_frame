/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 17:26:40
 */
#include "exti.h"

void (*itr[16])(void) = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

void attach_ITR(pin p, detect_mode dmode, void (*f)(void), NVIC_priority priority){
    pinMode(p, INPUT);
	itr[p.bitnum] = f;
    Ex_NVIC_Config((((u32)(p.reg)-APB2PERIPH_BASE))/0x0400-2, p.bitnum, dmode); 
    if(p.bitnum < 5){
        NVIC_init(EXTI0_IRQn + p.bitnum, priority); 
    }else if(p.bitnum < 10){
        NVIC_init(EXTI9_5_IRQn, priority); 
    }else{
        NVIC_init(EXTI15_10_IRQn, priority); 
    }
}

extern "C" void EXTI0_IRQHandler(void);
extern "C" void EXTI1_IRQHandler(void);
extern "C" void EXTI2_IRQHandler(void);
extern "C" void EXTI3_IRQHandler(void);
extern "C" void EXTI4_IRQHandler(void);
extern "C" void EXTI9_5_IRQHandler(void);
extern "C" void EXTI15_10_IRQHandler(void);

void EXTI0_IRQHandler(void){
    if(itr[0] != 0)
        itr[0]();		
	EXTI->PR = 1 << 0;
}
void EXTI1_IRQHandler(void){
    if(itr[1] != 0)
        itr[1]();
	EXTI->PR = 1 << 1;
}
void EXTI2_IRQHandler(void){
    if(itr[2] != 0)
        itr[2]();
	EXTI->PR = 1 << 2;
}
void EXTI3_IRQHandler(void){
    if(itr[3] != 0)
        itr[3]();
	EXTI->PR = 1 << 3;
}
void EXTI4_IRQHandler(void){
    if(itr[4] != 0)
        itr[4]();
	EXTI->PR = 1 << 4;
}
void EXTI9_5_IRQHandler(void){
	for(u8 i = 5; i < 10; i++){
		if(itr[i] != 0)
			itr[i]();
		EXTI->PR = 1 << i;
	}
}
void EXTI15_10_IRQHandler(void){
    for(u8 i = 10; i < 16; i++){
		if(itr[i] != 0)
			itr[i]();
		EXTI->PR = 1 << i;
	}
}



