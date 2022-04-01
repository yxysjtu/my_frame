#include "usart.h"

#ifdef __cplusplus
extern "C" {
#endif
	
UART uart1;
	
void USART1_IRQHandler(void){
	uart1.IRQHandler();
}

void DMA1_Channel4_IRQHandler(void){
	uart1.tdma_IRQHandler();
}

void DMA1_Channel5_IRQHandler(void){
	uart1.rdma_IRQHandler();
}

#ifdef __cplusplus
}
#endif
