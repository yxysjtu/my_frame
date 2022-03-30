/*
 * @Description: 时钟，GPIO，NVIC，基础位运算
 * @Author: yu
 * @LastEditTime: 2022-02-27 17:47:00
 */

#ifndef BASIC_H
#define BASIC_H 	
	
#include "sys.h"  

//priority
typedef struct {
	u8 group;
	u8 preemption_priority;
	u8 sub_priority;
} NVIC_priority;

//mode
typedef enum {
	FLOAT_INPUT = 0X04,
	OUTPUT = 0x03,
	OUTPUT_OPEN_DRAIN = 0X07,
	ALTERNATE_OUTPUT = 0X0B,
	ALTERNATE_OUTPUT_OPEN_DRAIN = 0X0F,
	INPUT = 0x08,
	ANALOG_INPUT = 0x00
}IO_mode;

typedef enum {
	LOW = 0,
	HIGH = 1,
	FLOAT = 2
}IO_level;


#define null ((void *)0)

typedef volatile unsigned long vul;

typedef struct{
	GPIO_TypeDef *reg;
	u8 bitnum;
}pin;

#define GPIOout(p)   BIT_ADDR((u32)p.reg + 12, p.bitnum)
#define GPIOin(p)	 (p.reg->IDR & (1 << p.bitnum)) >> p.bitnum
//#define GPIOin(p)   BIT_ADDR((u32)p.reg + 8, p.bitnum)

inline u8 read_reg_bit(vul *reg, u8 bitnum){return (*reg & (1 << bitnum)) != 0;}

extern u8 sys_clock, sys_clock_pll;
void sys_init(u8 clock=8, u8 pll=9);

//math
inline u32 put_bit(u32 num, u8 bits, u8 level){
	if(level) return num | (0x00000001<<bits); 
	return num & (~(0x00000001<<bits)); 
}
inline u32 put16(u32 num, u8 bits, u32 n){
	u32 t = ~(0x0000000f<<(bits*4));
	return (num&t)|(n<<bits*4);
}
inline u8 read_bit(u32 num, u8 b){
	return num & (0x00000001<<b); 
}
inline int round(float num){
	return (int)num + (int)((num - (int)num) / 0.5);
}


void delay(u16 ms);
void delay_us(u32 us);

void resetPinMode(pin p, IO_mode mode);
void pinMode(pin p, IO_mode mode, IO_level level=FLOAT);

	
#endif
