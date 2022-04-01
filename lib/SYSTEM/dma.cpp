/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-03-31 19:53:24
 */
#include "dma.h"

DMA_CH::DMA_CH(DMA_Channel_TypeDef* DMA_CHx){
    this->DMA_CHx = DMA_CHx;
	tc_flag = 1;
}

void DMA_CH::init(u32 paddr, u32 maddr, dma_dir dir, u16 data_len, u8 pinc, u8 minc, data_size psize, data_size msize, u8 m2m, u8 circ, dma_priority priority, u8 itr_enable){
    if((u32)DMA_CHx < DMA2_BASE) RCC->AHBENR |= 1 << 0;
    else RCC->AHBENR |= 1 << 1;
	delay(5);
	DMA_CHx->CPAR = paddr;
    DMA_CHx->CMAR = maddr;
    this->data_len = data_len;
    DMA_CHx->CCR = 0X00000000; //reset
    DMA_CHx->CCR |= (dir << 4) + (circ << 5) + (pinc << 6) + (minc << 7) + (psize << 8) + (msize << 10) + (priority << 12) + (m2m << 14);
    DMA_CHx->CCR |= itr_enable << 1; //TCIE
	
	if(itr_enable){
		switch((u32)DMA_CHx){
			case (u32)DMA1_Channel1:MY_NVIC_Init(3, 3, DMA1_Channel1_IRQn, 2); break;
			case (u32)DMA1_Channel2:MY_NVIC_Init(3, 3, DMA1_Channel2_IRQn, 2); break;
			case (u32)DMA1_Channel3:MY_NVIC_Init(3, 3, DMA1_Channel3_IRQn, 2); break;
			case (u32)DMA1_Channel4:MY_NVIC_Init(3, 3, DMA1_Channel4_IRQn, 2); break;
			case (u32)DMA1_Channel5:MY_NVIC_Init(3, 3, DMA1_Channel5_IRQn, 2); break;
			case (u32)DMA1_Channel6:MY_NVIC_Init(3, 3, DMA1_Channel6_IRQn, 2); break;
			case (u32)DMA1_Channel7:MY_NVIC_Init(3, 3, DMA1_Channel7_IRQn, 2); break;
			case (u32)DMA2_Channel1:MY_NVIC_Init(3, 3, DMA2_Channel1_IRQn, 2); break;
			case (u32)DMA2_Channel2:MY_NVIC_Init(3, 3, DMA2_Channel2_IRQn, 2); break;
			case (u32)DMA2_Channel3:MY_NVIC_Init(3, 3, DMA2_Channel3_IRQn, 2); break;
			case (u32)DMA2_Channel4:MY_NVIC_Init(3, 3, DMA2_Channel4_5_IRQn, 2); break;
			case (u32)DMA2_Channel5:MY_NVIC_Init(3, 3, DMA2_Channel4_5_IRQn, 2); break;
		}
	}
}
void DMA_CH::config(u32 paddr, u32 maddr, u16 data_len){
	DMA_CHx->CPAR = paddr;
    DMA_CHx->CMAR = maddr;
    this->data_len = data_len;
}

void DMA_CH::set_data_len(u16 len){
    data_len = len;
}
void DMA_CH::enable(){
	tc_flag = 0;
    DMA_CHx->CCR &= ~(1 << 0);
    DMA_CHx->CNDTR = data_len;
    DMA_CHx->CCR |= 1 << 0;
}
void DMA_CH::enable(u16 len){
	data_len = len;
	enable();
}
u8 DMA_CH::transmitt_complete(){
	if(!tc_flag) IRQHandler();
	return tc_flag;
}
void DMA_CH::disable(){
    DMA_CHx->CCR &= ~(1 << 0);
}
void DMA_CH::IRQHandler(void){
	if((u32)DMA_CHx < DMA2_BASE){
		u32 ch = ((u32)DMA_CHx - DMA1_Channel1_BASE) / 20;
		if(DMA1->ISR & (1 << (4*ch+1))){
			DMA1->IFCR |= 1 << (4*ch+1);
			tc_flag = 1;
		}
	}else{
		u32 ch = ((u32)DMA_CHx - DMA2_Channel1_BASE) / 20;
		if(DMA2->ISR & (1 << (4*ch+1))){
			DMA2->IFCR |= 1 << (4*ch+1);
			tc_flag = 1;
		}
	}
}