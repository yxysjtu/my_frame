#include "usart.h"
#include "timer.h"
#include "led.h"

UART uart1;
timer tim1(TIM1);
timer tim2(TIM2);
timer tim3(TIM3);
timer tim4(TIM4);
timer tim5(TIM5);
timer tim6(TIM6);
timer tim7(TIM7);
timer tim8(TIM8);

#ifdef __cplusplus
extern "C" {
#endif
	
void USART1_IRQHandler(void){
	uart1.IRQHandler();
}

void DMA1_Channel4_IRQHandler(void){
	uart1.tdma_IRQHandler();
}

void DMA1_Channel5_IRQHandler(void){
	uart1.rdma_IRQHandler();
}

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
