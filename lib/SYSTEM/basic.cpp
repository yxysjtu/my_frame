/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 17:25:31
 */
#include "basic.h"
#include "timer.h"
#include "led.h"

NVIC_priority default_priority = {2,2,3};
NVIC_priority default_priority_H = {2,1,3};

u32 sys_ms = 0;

//init clock and systick
extern "C"{
void SysTick_Handler(){
	sys_ms++;
}
}
u8 sys_clock = 8, sys_clock_pll = 9; //8M
void sys_init(u8 clock, u8 pll){
	sys_clock = clock;
	sys_clock_pll = pll;
	Stm32_Clock_Init(pll); 
	
	MY_NVIC_PriorityGroupConfig(0);   
	SysTick_Config(72000);
}

void NVIC_init(u8 channel, NVIC_priority priority){
	MY_NVIC_Init(priority.preemption_priority, priority.sub_priority, channel, priority.group); 
}


//timer
u32 millis(){return sys_ms;}
u32 micros(){return sys_ms * 1000 + (72000 - SysTick->VAL) / 72;}

void delay(u32 ms){
	u32 t0 = sys_ms;
	while(sys_ms - t0 < ms);
}
void delay_us(u32 us){ //72000 1/72us 72tick 1us
	u32 t0 = SysTick->VAL;
	u32 tick = 72 * us;
	u32 tms0 = sys_ms;
	while(1){
		if(SysTick->VAL != 0)
			if(((sys_ms - tms0) * 72000 + t0 - SysTick->VAL) >= tick) break;
	}
}


//IO
void resetPinMode(pin p, IO_mode mode){
	GPIO_TypeDef *GPIO = p.reg;
	if(p.bitnum<8){
		GPIO->CRL=put16(GPIO->CRL,p.bitnum,mode);
	}else{
		GPIO->CRH=put16(GPIO->CRH,p.bitnum-8,mode);
	}
}

void pinMode(pin p, IO_mode mode, IO_level level){
	GPIO_TypeDef *GPIO = p.reg;
	RCC->APB2ENR|=1<<(((u32)GPIO-APB2PERIPH_BASE))/0x0400; 
	
	if(p.bitnum < 8){
		GPIO->CRL=put16(GPIO->CRL,p.bitnum,mode);
	}else{
		GPIO->CRH=put16(GPIO->CRH,p.bitnum-8,mode);
	}
	if(level == HIGH) GPIO->ODR |= 1 << p.bitnum;
	else GPIO->ODR &= ~(1 << p.bitnum);
}

u8 isnum(char c){
	return c >= '0' && c <= '9';
}
int to_int(u8* s, u8 len){
    int n = 0;	
    unsigned char sign = 0;
	if(!isnum(s[0])){
		if(s[0] == '-') sign = 1;
		else if(s[0] == '+') sign = 0;
		else return -1;
	}
    for(u8 i = sign; i < len; i++){
        n *= 10;
        if(isnum(s[i])) n += s[i] - '0';
		else return -1;
    }
    if(sign) n = -n;
    return n;
}
float to_float(u8* s, u8 len){
    float n = 0;
    unsigned char sign = 0;
	if(!isnum(s[0])){
		if(s[0] == '-') sign = 1;
		else if(s[0] == '+') sign = 0;
		else return -1;
	}
	u8 i;
    for(i = sign; i < len; i++){
        n *= 10;
        if(isnum(s[i])) n += s[i] - '0';
		else if(s[i] == '.') break;
		else return -1;
    }
    float n1 = 0;
    for(int j = len - 1; j > i; j--){
        if(isnum(s[j])) n1 += s[j] - '0';
		else return -1;
        n1 /= 10;
    }
    n += n1;
    if(sign) n = -n;
    return n;
}

