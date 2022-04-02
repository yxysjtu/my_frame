/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 17:25:31
 */
#include "basic.h"
#include "timer.h"

NVIC_priority default_priority = {2,2,3};
NVIC_priority default_priority_H = {2,1,3};

u32 sys_ms = 0;
void update_time(){
	sys_ms++;
}
//init clock and systick
u8 sys_clock = 8, sys_clock_pll = 9; //8M
void sys_init(u8 clock, u8 pll){
	sys_clock = clock;
	sys_clock_pll = pll;
	Stm32_Clock_Init(pll); 
	SysTick->CTRL &= ~(1<<2);	
	
	tim1.init();
	tim1.set_frequency(1000); //1k
	tim1.attach_ITR(update_time);
	tim1.enable();
}

void NVIC_init(u8 channel, NVIC_priority priority){
	MY_NVIC_Init(priority.preemption_priority, priority.sub_priority, channel, priority.group); 
}


u32 millis(){return sys_ms;}

//delay
void delay(u16 ms){
	u32 temp;		   
	SysTick->LOAD=1000*1000*sys_clock_pll;		
	SysTick->VAL =0x00;           			
	SysTick->CTRL=0x01 ;          			
	for(u8 i=0;i<ms/1000;i++){  
		do
		{
			temp=SysTick->CTRL;
		}while((temp&0x01)&&!(temp&(1<<16)));	  
	}
	SysTick->CTRL=0x00;      	 	
	if(ms%1000>0){
		SysTick->LOAD=(ms%1000)*1000*sys_clock_pll;		
		SysTick->VAL =0x00;           		
		SysTick->CTRL=0x01 ;          	
		do{
			temp=SysTick->CTRL;
		}while((temp&0x01)&&!(temp&(1<<16)));	 
		SysTick->CTRL=0x00;      	 			
	}
	SysTick->VAL =0X00;       			
}
void delay_us(u32 us){
	u32 temp;	    	 
	SysTick->LOAD=us*sys_clock_pll; //1/9us		 
	SysTick->VAL=0x00;        		
	SysTick->CTRL=0x01;      		 
	do{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	
	SysTick->CTRL=0x00;      	 		
	SysTick->VAL =0x00;       		
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
	if(level == 1) GPIO->ODR |= 1<<p.bitnum;
	else if(level == 0) GPIO->ODR &= ~(1<<p.bitnum);
}
