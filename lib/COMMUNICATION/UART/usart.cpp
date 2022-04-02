#include "usart.h"	 
//APB2:UARST1 72M
//APB1:UARST2~5 36M

UART_pin default_uart_pin = { //uart1
	{GPIOA, 10},
	{GPIOA, 9}
};

UART uart1(USART1), uart2(USART2), uart3(USART3);
#ifdef UART4
UART uart4(UART4);
#endif
#ifdef UART5
UART uart5(UART5);
#endif

void tdma1_IRQHandler(void){
	uart1.tc_flag = 1;
}
void rdma1_IRQHandler(void){
	uart1.USART_RX_STA |= 1 << 15;
}
void tdma2_IRQHandler(void){
	uart2.tc_flag = 1;
}
void rdma2_IRQHandler(void){
	uart2.USART_RX_STA |= 1 << 15;
}
void tdma3_IRQHandler(void){
	uart3.tc_flag = 1;
}
void rdma3_IRQHandler(void){
	uart3.USART_RX_STA |= 1 << 15;
}

	
UART::UART(USART_TypeDef * uart){
	this->USART = uart;
	tc_flag = 1;
	USART_RX_STA = 0;
	
	if(uart == USART2){
		rdma = &dma1_6;
		tdma = &dma1_7;
	}else if(uart == USART3){
		rdma = &dma1_3;
		tdma = &dma1_2;
	}else if(uart == USART1){
		rdma = &dma1_5;
		tdma = &dma1_4;
	}	
}

void UART::enable(UART_pin p, recv_mode rmode, io_t rx_way, io_t tx_way, u32 baudrate, parity_e parity){
	this->rmode = rmode;
	this->rx_way = rx_way;
	this->tx_way = tx_way;
	
	float uartdiv;
	u16 div_mantissa, div_fraction;
	u8 pclk = sys_clock * sys_clock_pll;
	
	if(USART == USART1){
		RCC->APB2RSTR |= 1 << 14;   
		RCC->APB2RSTR &= ~(1 << 14);
		RCC->APB2ENR |= 1 << 14; 
	}else{
		pclk /= 2; //APB1
		RCC->APB1RSTR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);   
		RCC->APB1RSTR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));
		RCC->APB1ENR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);  
	}
	//set baudrate
	uartdiv = (float)(pclk * 1000000) / (baudrate * 16);
	div_mantissa = uartdiv;			
	div_fraction = (u16)round((uartdiv - div_mantissa) * 16); 
    USART->BRR = (div_mantissa << 4) + div_fraction;
	USART->CR1 |= 0X200C; 
	USART->CR1 |= parity << 9; //set parity control
	if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	
	if(tx_way == DMA){
		USART->CR3 |= 1 << 7; //ENABLE DMA TRANSMITT
		tdma->init((u32)&(USART->DR), (u32)tx_buf, m2p, USART_REC_LEN, 0, 1, bit_8, bit_8, 0, 0, priority_medium);
		switch((u32)USART){
			case USART1_BASE: tdma->attach_ITR(tdma1_IRQHandler); break;
			case USART2_BASE: tdma->attach_ITR(tdma2_IRQHandler); break;
			case USART3_BASE: tdma->attach_ITR(tdma3_IRQHandler); break;
			default: break;
		}
		
	}
	if(rx_way == ITR) USART->CR1 |= 1 << 5;    //RXNEIE 
	else if(rx_way == DMA){
		USART->CR3 |= 1 << 6; //ENABLE DMA RECV
		rdma->init((u32)&(USART->DR), (u32)USART_RX_BUF, p2m, USART_REC_LEN, 0, 1, bit_8, bit_8, 0, 0, priority_medium);
		switch((u32)USART){
			case USART1_BASE: rdma->attach_ITR(rdma1_IRQHandler); break;
			case USART2_BASE: rdma->attach_ITR(rdma2_IRQHandler); break;
			case USART3_BASE: rdma->attach_ITR(rdma3_IRQHandler); break;
			default: break;
		}
	}
	
	pinMode(p.rx, INPUT);
	pinMode(p.tx, ALTERNATE_OUTPUT);
	
	if((rmode != RECV_OFF && rx_way == ITR) || tx_way == ITR || rmode == RECV_BY_TIME_SEPRAITON){
		switch((u32)USART){
			case (u32)USART1: MY_NVIC_Init(3, 3, USART1_IRQn, 2); break; 
			case (u32)USART2: MY_NVIC_Init(3, 3, USART2_IRQn, 2); break; 
			case (u32)USART3: MY_NVIC_Init(3, 3, USART3_IRQn, 2); break;   
			#ifdef UART4
			case (u32)UART4: MY_NVIC_Init(3, 3, UART4_IRQn, 2); break; 
			#endif
			#ifdef UART5
			case (u32)UART5: MY_NVIC_Init(3, 3, UART5_IRQn, 2); break;
			#endif
		}	
	}
	
}

void UART::printf(const char *fmt,...){
    static va_list ap;
	
    va_start(ap,fmt);
    tx_len = vsprintf((char*)tx_buf,fmt,ap);
    va_end(ap);
	
	ti = 0;
	tc_flag = 0;
	if(tx_way == BLOCK){
		for(; ti < tx_len; ti++){
			while((USART->SR & 0X40) == 0);
			USART->DR = tx_buf[ti];    
		}
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		ti = 0;
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		USART->DR = tx_buf[0];
	}else if(tx_way == DMA){
		tdma->config((u32)&(USART->DR), (u32)tx_buf, tx_len);
		tdma->enable();
	}
		
}


u8 UART::IRQHandler(void){ //receive interrupt
	u8 res;	
	if(USART->SR & (1 << 4)){ //detect idle line
		USART_RX_STA |= 1 << 15;
		res = USART->DR; 
	}
	else if(USART->SR & (1 << 5)){ //recv	 
		res = USART->DR; 
		switch(rmode){
			case RECV_OFF: break;
			case RECV_BY_LINE_ENDING:
				if((USART_RX_STA & 0x8000)==0){
					if(USART_RX_STA & 0x4000){
						if(res != 0x0a) USART_RX_STA = 0;
						else USART_RX_STA |= 0x8000;	
					}else{ 
						if(res == 0x0d) USART_RX_STA |= 0x4000;
						else{
							USART_RX_BUF[USART_RX_STA&0X3FFF] = res;
							USART_RX_STA++; 
							if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;
						}		 
					}
				} 
				break;  	
			case RECV_BY_TIME_SEPRAITON:
				if(!(USART_RX_STA & 0x8000)){
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = res;
					USART_RX_STA++; 
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0; 
				}
				break;
			case RECV_IN_CIRCULAR_BUF: 
				if(ri2 < ri1 || !rovf){ //when buf full, discard recv data
					USART_RX_BUF[ri2] = res;
					if((++ri2) >= USART_REC_LEN){
						ri2 = 0;
						rovf = 1;
					}
				}
				break;
			case RECV_CUSTOMIZE: break;
		}
	}
	else if(USART->SR & (1 << 6)){
		if(ti < tx_len - 1){
			ti++;
			USART->DR = tx_buf[ti];
		}else{
			tc_flag = 1;
			USART->CR1 &= ~(1 << 6); //TCIE
			USART->SR &= ~(1 << 6); //to avoid still set to 1 and then trigger itr when tcie
		}
	}
	
	return res;
} 


u8 UART::readline(u8 *buf, u8 *len, u32 timeout){
	if(rmode == RECV_BY_LINE_ENDING || rmode == RECV_BY_TIME_SEPRAITON){
		if(rx_way == BLOCK){
			u8 res;
			u32 i;
			//USART_RX_STA = 0;
			while((USART_RX_STA & 0x8000)==0){
				for(i = 0; i/5000 < timeout && !(USART->SR & (1 << 5)) && !(USART_RX_STA & 0x8000); i++);
				if(i/5000 >= timeout) return 0;
				if(USART_RX_STA & 0x8000) break;
				res = USART->DR;

				if(USART_RX_STA & 0x4000){
					if(res != 0x0a) USART_RX_STA = 0;
					else USART_RX_STA |= 0x8000;	
				}else{ 
					if(res == 0x0d && (rmode != RECV_BY_TIME_SEPRAITON)) USART_RX_STA |= 0x4000;
					else{
						buf[USART_RX_STA&0X3FFF] = res;
						USART_RX_STA++; 
						if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;  
					}		 
				}
			} 
			*len = USART_RX_STA&0X3FFF;
			USART_RX_STA = 0;
			return 1;
		}else if(rx_way == ITR){
			if(USART_RX_STA & 0x8000){
				*len = USART_RX_STA & 0x3fff;
				for(u8 i = 0; i < *len; i++){
					buf[i] = USART_RX_BUF[i];
				}
				USART_RX_STA = 0;
				return 1;
			}
		}else if(rx_way == DMA){ //only support idle
			if(USART_RX_STA & 0x8000){
				*len = rdma->data_len - rdma->DMA_CHx->CNDTR;
				rdma->tc_flag = 1;
				USART_RX_STA = 0;
				return 1;
			}else if(rdma->tc_flag){ //initial config
				rdma->config((u32)&(USART->DR), (u32)buf, USART_REC_LEN);
				rdma->enable();
			}
			return 0;
		}
		
	}else if(rmode == RECV_IN_CIRCULAR_BUF){ //dma not supported
		//read all data in buf
		if(ri1 != ri2 || rovf){
			USART->CR1 &= ~((1 << 5)); //stop recv		
			*len = 0;
			while((ri1 < ri2) || rovf){
				buf[*len] = USART_RX_BUF[ri1];
				(*len)++; 
				if((++ri1) >= USART_REC_LEN){
					ri1 = 0;
					rovf = 0;
				}
			}
			USART->CR1 |= (1 << 5); //enable recv
			return 1;
		}
	}
	
	return 0;
}


u8 UART::read(u8 *buf, u16 len){
	if(rx_way == DMA){
		if(USART_RX_STA & 0x8000){
			USART_RX_STA = 0;
			return 1;
		}else if(rdma->tc_flag){
			rdma->config((u32)&(USART->DR), (u32)buf, len);
			rdma->enable();
		}
		return 0;
	}
	else if(rmode == RECV_IN_CIRCULAR_BUF){
		u16 len0 = ri2 + rovf * USART_REC_LEN - ri1;
		if((len0 >= len) && (ri1 != ri2 || rovf)){
			USART->CR1 &= ~((1 << 5)); //stop recv
			
			u8 i = 0;
			while(((ri1 < ri2) || rovf) && i < len0){
				buf[i] = USART_RX_BUF[ri1];
				i++; 
				if((++ri1) >= USART_REC_LEN){
					ri1 = 0;
					rovf = 0;
				}
			}
			
			USART->CR1 |= (1 << 5); //enable recv
			return 1;
		}
	}
	
	return 0;
}

void UART::write(u8 val){
	tc_flag = 0;
	tx_len = 1;

	if(tx_way == BLOCK){
		while((USART->SR & 0X40) == 0);
		USART->DR = val;   
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		ti = 0;
		USART->DR = val;
	}else if(tx_way == DMA){ //I think no one will use DMA to send only 1 bit
		tx_buf[0] = val;
		tdma->config((u32)&(USART->DR), (u32)tx_buf, tx_len);
		tdma->enable();
	}
	
}

void UART::write(u8 *buf, u32 len){
	tc_flag = 0;
	tx_len = len;

	if(tx_way == BLOCK){
		for(u32 i = 0; i < len; i++){
			write(buf[i]);
		}
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		for(u16 i = 0; i < len; i++) tx_buf[i] = buf[i];
		ti = 0;
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		USART->DR = tx_buf[0];
	}else if(tx_way == DMA){
		tdma->config((u32)&(USART->DR), (u32)buf, tx_len);
		tdma->enable();
	}
	
}

u8 UART::write_available(){
	return tc_flag;
}

void UART::disable(){
	if(USART == USART1){
		RCC->APB2ENR &= ~(1 << 14);  
		RCC->APB2RSTR |= 1 << 14;  
		RCC->APB2RSTR &= ~(1 << 14);	 
	}else{
		RCC->APB1ENR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400)); 
		RCC->APB1RSTR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);  
		RCC->APB1RSTR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));
	}
}

#ifdef __cplusplus
extern "C" {
#endif
	
void USART1_IRQHandler(void){
	uart1.IRQHandler();
}

void USART2_IRQHandler(void){
	uart2.IRQHandler();
}

void USART3_IRQHandler(void){
	uart3.IRQHandler();
}

#ifdef UART4
void UART4_IRQHandler(void){
	uart4.IRQHandler();
}
#endif
#ifdef UART5
void UART5_IRQHandler(void){
	uart5.IRQHandler();
}
#endif

#ifdef __cplusplus
}
#endif