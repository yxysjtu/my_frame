#include "usart.h"	 
//APB2:UARST1 72M
//APB1:UARST2~5 36M

		
UART_pin default_uart_pin = {
	{GPIOA, 10},
	{GPIOA, 9}
};
	
UART::UART(USART_TypeDef * uart, UART_pin p){
	this->USART = uart;
	this->p = p;
	tc_flag = 1;
	USART_RX_STA = 0;
	
	if(uart == USART2){
		rdma = DMA_CH(DMA1_Channel6);
		tdma = DMA_CH(DMA1_Channel7);
	}else if(uart == USART3){
		rdma = DMA_CH(DMA1_Channel3);
		tdma = DMA_CH(DMA1_Channel2);
	}else if(uart == USART1){
		rdma = DMA_CH(DMA1_Channel5);
		tdma = DMA_CH(DMA1_Channel4);
	}
	
}

void UART::begin(recv_mode rmode, io_t rx_way, io_t tx_way, u32 baudrate, parity_e parity){
	this->rmode = rmode;
	this->rx_way = rx_way;
	this->tx_way = tx_way;
	
	float uartdiv;
	u16 div_mantissa, div_fraction;
	u8 pclk = sys_clock * sys_clock_pll;
	
	if(USART == USART1){
		RCC->APB2RSTR |= 1 << 14;   //��λ����1
		RCC->APB2RSTR &= ~(1 << 14);//ֹͣ��λ	 
		RCC->APB2ENR |= 1 << 14;  //ʹ�ܴ���ʱ�� 
	}else{
		pclk /= 2; //APB1
		RCC->APB1RSTR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);   //��λ����
		RCC->APB1RSTR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));//ֹͣ��λ
		RCC->APB1ENR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);  //ʹ�ܴ���ʱ�� 
	}
	//����������
	uartdiv = (float)(pclk * 1000000) / (baudrate * 16);//�õ�USARTDIV
	div_mantissa = uartdiv;				 //�õ���������
	div_fraction = (u16)round((uartdiv - div_mantissa) * 16); //�õ�С������	 
    USART->BRR = (div_mantissa << 4) + div_fraction;
	USART->CR1 |= 0X200C;  //ʹ��, RE,TE, 1λֹͣ,��У��λ.
	USART->CR1 |= parity << 9; //set parity control
	if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	
	if(tx_way == DMA){
		USART->CR3 |= 1 << 7; //ENABLE DMA TRANSMITT
		tdma.init((u32)&(USART->DR), (u32)tx_buf, m2p, USART_REC_LEN, 0, 1, bit_8, bit_8, 0, 0, priority_medium, 1);
	}
	if(rx_way == ITR) USART->CR1 |= 1 << 5;    //RXNEIE 
	else if(rx_way == DMA){
		USART->CR3 |= 1 << 6; //ENABLE DMA RECV
		rdma.init((u32)&(USART->DR), (u32)USART_RX_BUF, p2m, USART_REC_LEN, 0, 1, bit_8, bit_8, 0, 0, priority_medium, 1);
	}
	
	pinMode(p.rx, INPUT);
	pinMode(p.tx, ALTERNATE_OUTPUT);
	
	if((rmode != RECV_OFF && rx_way == ITR) || tx_way == ITR || rmode == RECV_BY_TIME_SEPRAITON){
		switch((u32)USART){
			#ifdef USART1
				case (u32)USART1: MY_NVIC_Init(3, 3, USART1_IRQn, 2); break; //��2��������ȼ�  
			#endif
			#ifdef USART2
				case (u32)USART2: MY_NVIC_Init(3, 3, USART2_IRQn, 2); break; //��2��������ȼ�  
			#endif
			#ifdef USART3
				case (u32)USART3: MY_NVIC_Init(3, 3, USART3_IRQn, 2); break; //��2��������ȼ�  
			#endif
			#ifdef UART4
				case (u32)UART4: MY_NVIC_Init(3, 3, UART4_IRQn, 2); break; //��2��������ȼ�  
			#endif
			#ifdef UART5
				case (u32)UART5: MY_NVIC_Init(3, 3, UART5_IRQn, 2); break; //��2��������ȼ�  
			#endif
		}	
	}
	
}

void UART::printf(const char *fmt,...){ //��������
    static va_list ap;
	
    va_start(ap,fmt);
    tx_len = vsprintf((char*)tx_buf,fmt,ap);
    va_end(ap);
	
	ti = 0;
	tc_flag = 0;
	if(tx_way == BLOCK){
		for(; ti < tx_len; ti++){
			while((USART->SR & 0X40) == 0);//�ȴ���һ�δ������ݷ������  
			USART->DR = tx_buf[ti];      	//дDR,����1����������
		}
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		ti = 0;
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		USART->DR = tx_buf[0];
	}else if(tx_way == DMA){
		tdma.config((u32)&(USART->DR), (u32)tx_buf, tx_len);
		tdma.enable();
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
				if((USART_RX_STA & 0x8000)==0){//����δ���
					if(USART_RX_STA & 0x4000){//���յ���0x0d
						if(res != 0x0a) USART_RX_STA = 0;//���մ���,���¿�ʼ
						else USART_RX_STA |= 0x8000;	//��������� 
					}else{ //��û�յ�0X0D	
						if(res == 0x0d) USART_RX_STA |= 0x4000;
						else{
							USART_RX_BUF[USART_RX_STA&0X3FFF] = res;
							USART_RX_STA++; 
							if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
						}		 
					}
				} 
				break;  	
			case RECV_BY_TIME_SEPRAITON:
				if(!(USART_RX_STA & 0x8000)){
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = res;
					USART_RX_STA++; 
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	 
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

void UART::tdma_IRQHandler(void){
	if(tdma.transmitt_complete()){
		tc_flag = 1;
		tdma.disable();
	}
}
void UART::rdma_IRQHandler(void){
	if(rdma.transmitt_complete()){
		USART_RX_STA |= 1 << 15;
		rdma.disable();
	}
}

u8 UART::readline(u8 *buf, u8 *len, u32 timeout){
	if(rmode == RECV_BY_LINE_ENDING || rmode == RECV_BY_TIME_SEPRAITON){
		if(rx_way == BLOCK){
			u8 res;
			u32 i;
			//USART_RX_STA = 0;
			while((USART_RX_STA & 0x8000)==0){//����δ���
				for(i = 0; i/5000 < timeout && !(USART->SR & (1 << 5)) && !(USART_RX_STA & 0x8000); i++);
				if(i/5000 >= timeout) return 0;
				if(USART_RX_STA & 0x8000) break;
				res = USART->DR;

				if(USART_RX_STA & 0x4000){//���յ���0x0d
					if(res != 0x0a) USART_RX_STA = 0;//���մ���,���¿�ʼ
					else USART_RX_STA |= 0x8000;	//��������� 
				}else{ //��û�յ�0X0D	
					if(res == 0x0d && (rmode != RECV_BY_TIME_SEPRAITON)) USART_RX_STA |= 0x4000;
					else{
						buf[USART_RX_STA&0X3FFF] = res;
						USART_RX_STA++; 
						if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;//�������ݴ���,���¿�ʼ����	  
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
				*len = rdma.data_len - rdma.DMA_CHx->CNDTR;
				rdma.tc_flag = 1;
				USART_RX_STA = 0;
				return 1;
			}else if(rdma.tc_flag){ //initial config
				rdma.config((u32)&(USART->DR), (u32)buf, USART_REC_LEN);
				rdma.enable();
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
		}else if(rdma.tc_flag){
			rdma.config((u32)&(USART->DR), (u32)buf, len);
			rdma.enable();
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
		tdma.config((u32)&(USART->DR), (u32)tx_buf, tx_len);
		tdma.enable();
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
		tdma.config((u32)&(USART->DR), (u32)buf, tx_len);
		tdma.enable();
	}
	
}

u8 UART::write_available(){
	return tc_flag;
}

void UART::disable(){
	if(USART == USART1){
		RCC->APB2ENR &= ~(1 << 14);  //�رմ���ʱ�� 
		RCC->APB2RSTR |= 1 << 14;   //��λ����1
		RCC->APB2RSTR &= ~(1 << 14);//ֹͣ��λ	 
	}else{
		RCC->APB1ENR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));  //�رմ���ʱ�� 
		RCC->APB1RSTR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);   //��λ����
		RCC->APB1RSTR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));//ֹͣ��λ
	}
}
