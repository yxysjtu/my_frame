/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 18:25:50
 */
#include "basic.h"

//init clock and systick
u8 sys_clock = 8, sys_clock_pll = 9; //8M

void sys_init(u8 clock, u8 pll){
	sys_clock = clock;
	sys_clock_pll = pll;
	Stm32_Clock_Init(pll); 
	SysTick->CTRL &= ~(1<<2);	
}


//delay
void delay(u16 ms){
	u32 temp;		   
	SysTick->LOAD=1000*1000*sys_clock_pll;			//ʱ�����(SysTick->LOADΪ24bit
	SysTick->VAL =0x00;           			//��ռ�����
	SysTick->CTRL=0x01 ;          			//��ʼ����
	for(u8 i=0;i<ms/1000;i++){  
		do
		{
			temp=SysTick->CTRL;
		}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	}
	SysTick->CTRL=0x00;      	 			//�رռ�����
	if(ms%1000>0){
		SysTick->LOAD=(ms%1000)*1000*sys_clock_pll;			//ʱ�����(SysTick->LOADΪ24bit
		SysTick->VAL =0x00;           			//��ռ�����
		SysTick->CTRL=0x01 ;          			//��ʼ����  
		do
		{
			temp=SysTick->CTRL;
		}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
		SysTick->CTRL=0x00;      	 			//�رռ�����
	}
	SysTick->VAL =0X00;       				//��ռ�����
}
void delay_us(u32 us){
	u32 temp;	    	 
	SysTick->LOAD=us*sys_clock_pll; 					//ʱ�����,1/9us		 
	SysTick->VAL=0x00;        				//��ռ�����
	SysTick->CTRL=0x01;      				//��ʼ���� 	 
	do
	{
		temp=SysTick->CTRL;
	}while((temp&0x01)&&!(temp&(1<<16)));	//�ȴ�ʱ�䵽��   
	SysTick->CTRL=0x00;      	 			//�رռ�����
	SysTick->VAL =0x00;       				//��ռ�����	
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
	RCC->APB2ENR|=1<<(((u32)GPIO-APB2PERIPH_BASE))/0x0400; //ʹ�� PORT ʱ��
	
	if(p.bitnum < 8){
		GPIO->CRL=put16(GPIO->CRL,p.bitnum,mode);
	}else{
		GPIO->CRH=put16(GPIO->CRH,p.bitnum-8,mode);
	}
	if(level == 1) GPIO->ODR |= 1<<p.bitnum;
	else if(level == 0) GPIO->ODR &= ~(1<<p.bitnum);
}
