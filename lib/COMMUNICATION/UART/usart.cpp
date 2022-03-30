#include "usart.h"	 
//APB2:UARST1 72M
//APB1:UARST2~5 36M
//add DMA in the future


UART_pin default_uart_pin = {
	{GPIOA, 10},
	{GPIOA, 9}
};
	
UART::UART(recv_mode rmode, USART_TypeDef * uart, UART_pin p){
	this->USART = uart;
	this->p = p;
	this->rmode = rmode;
}

void UART::begin(u32 baudrate, parity_e parity){
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
	
	pinMode(p.rx, INPUT);
	pinMode(p.tx, ALTERNATE_OUTPUT);	   	   
	
	if(rmode != RECV_OFF){
		//ʹ�ܽ����ж� 
		USART->CR1 |= 1 << 5;    //���ջ������ǿ��ж�ʹ��	   
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
    static uint8_t tx_buf[USART_REC_LEN] = {0};
    static va_list ap;
    static uint16_t len;
	u8 *p_buf = tx_buf;
	
    va_start(ap,fmt);

    len = vsprintf((char*)tx_buf,fmt,ap);

    va_end(ap);

	for(u8 i = 0; i < len; i++){
		while((USART->SR & 0X40) == 0);//�ȴ���һ�δ������ݷ������  
		USART->DR = *p_buf;      	//дDR,����1����������
		p_buf++;
	}
}


u8 UART::IRQHandler(void){ //receive
	u8 res;	
	if(USART->SR & (1 << 4)){ //detect idle line
		USART_RX_STA |= 1 << 15;
		res = USART->DR; 
	}
	else if(USART->SR & (1 << 5))	//���յ�����
	{	 
		res = USART->DR; 
		switch(rmode){
			case RECV_OFF: break;
			case RECV_BY_LINE_ENDING:
				if((USART_RX_STA & 0x8000)==0)//����δ���
				{
					if(USART_RX_STA & 0x4000)//���յ���0x0d
					{
						if(res != 0x0a) USART_RX_STA = 0;//���մ���,���¿�ʼ
						else USART_RX_STA |= 0x8000;	//��������� 
					}else //��û�յ�0X0D
					{	
						if(res == 0x0d) USART_RX_STA |= 0x4000;
						else
						{
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
	
	//USART->SR &= ~(1<<5);
	return res;
} 
	
u8 UART::readline(u8 *buf, u8 *len){
	if(rmode == RECV_BY_LINE_ENDING || rmode == RECV_BY_TIME_SEPRAITON){
		if(USART_RX_STA & 0x8000){
			*len = USART_RX_STA & 0x3fff;
			for(u8 i = 0; i < *len; i++){
				buf[i] = USART_RX_BUF[i];
			}
			USART_RX_STA = 0;
			return 1;
		}
	}else if(rmode == RECV_IN_CIRCULAR_BUF){
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


u8 UART::read(u8 *buf, u16 len){ //just for circ_buf, len can't exceed buf_len
	if(rmode == RECV_IN_CIRCULAR_BUF){
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
	while((USART->SR & 0X40) == 0);//�ȴ���һ�δ������ݷ������  
	USART->DR = val;      	//дDR,����1����������
}

void UART::write(u8 *buf, u32 len){
	for(u32 i = 0; i < len; i++){
		write(buf[i]);
	}
}
