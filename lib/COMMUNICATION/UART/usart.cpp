#include "usart.h"	 
//APB2:UARST1 72M
//APB1:UARST2~5 36M
//add DMA in the future
#include "led.h"

		
UART_pin default_uart_pin = {
	{GPIOA, 10},
	{GPIOA, 9}
};
	
UART::UART(recv_mode rmode, io_t rx_way, io_t tx_way, USART_TypeDef * uart, UART_pin p){
	this->USART = uart;
	this->p = p;
	this->rmode = rmode;
	this->rx_way = rx_way;
	this->tx_way = tx_way;
	tc_flag = 1;
	USART_RX_STA = 0;
}

void UART::begin(u32 baudrate, parity_e parity){
	float uartdiv;
	u16 div_mantissa, div_fraction;
	u8 pclk = sys_clock * sys_clock_pll;
	
	if(USART == USART1){
		RCC->APB2RSTR |= 1 << 14;   //复位串口1
		RCC->APB2RSTR &= ~(1 << 14);//停止复位	 
		RCC->APB2ENR |= 1 << 14;  //使能串口时钟 
	}else{
		pclk /= 2; //APB1
		RCC->APB1RSTR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);   //复位串口
		RCC->APB1RSTR &= ~(1 << (17 + ((u32)USART - USART2_BASE) / 0X0400));//停止复位
		RCC->APB1ENR |= 1 << (17 + ((u32)USART - USART2_BASE) / 0X0400);  //使能串口时钟 
	}
	//波特率设置
	uartdiv = (float)(pclk * 1000000) / (baudrate * 16);//得到USARTDIV
	div_mantissa = uartdiv;				 //得到整数部分
	div_fraction = (u16)round((uartdiv - div_mantissa) * 16); //得到小数部分	 
    USART->BRR = (div_mantissa << 4) + div_fraction;
	USART->CR1 |= 0X200C;  //使能, RE,TE, 1位停止,无校验位.
	USART->CR1 |= parity << 9; //set parity control
	if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	
	if(tx_way == DMA){
		
	}
	if(rx_way == ITR)
		USART->CR1 |= 1 << 5;    //RXNEIE 
	else if(rx_way == DMA){
			
	}
	
	pinMode(p.rx, INPUT);
	pinMode(p.tx, ALTERNATE_OUTPUT);
	
	if((rmode != RECV_OFF && rx_way == ITR) || tx_way == ITR || rmode == RECV_BY_TIME_SEPRAITON){
		switch((u32)USART){
			#ifdef USART1
				case (u32)USART1: MY_NVIC_Init(3, 3, USART1_IRQn, 2); break; //组2，最低优先级  
			#endif
			#ifdef USART2
				case (u32)USART2: MY_NVIC_Init(3, 3, USART2_IRQn, 2); break; //组2，最低优先级  
			#endif
			#ifdef USART3
				case (u32)USART3: MY_NVIC_Init(3, 3, USART3_IRQn, 2); break; //组2，最低优先级  
			#endif
			#ifdef UART4
				case (u32)UART4: MY_NVIC_Init(3, 3, UART4_IRQn, 2); break; //组2，最低优先级  
			#endif
			#ifdef UART5
				case (u32)UART5: MY_NVIC_Init(3, 3, UART5_IRQn, 2); break; //组2，最低优先级  
			#endif
		}	
	}
	
}

void UART::printf(const char *fmt,...){ //持续发送
    static va_list ap;
	
    va_start(ap,fmt);
    tx_len = vsprintf((char*)tx_buf,fmt,ap);
    va_end(ap);
	
	ti = 0;
	tc_flag = 0;
	//USART->CR1 &= ~(1 << 4); //IDLE
	if(tx_way == BLOCK){
		for(; ti < tx_len; ti++){
			while((USART->SR & 0X40) == 0);//等待上一次串口数据发送完成  
			USART->DR = tx_buf[ti];      	//写DR,串口1将发送数据
		}
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		ti = 0;
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		//USART->CR1 &= ~(1 << 5); //RXIE
		USART->DR = tx_buf[0];
	}
}


u8 UART::IRQHandler(void){ //receive
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
				if((USART_RX_STA & 0x8000)==0){//接收未完成
					if(USART_RX_STA & 0x4000){//接收到了0x0d
						if(res != 0x0a) USART_RX_STA = 0;//接收错误,重新开始
						else USART_RX_STA |= 0x8000;	//接收完成了 
					}else{ //还没收到0X0D	
						if(res == 0x0d) USART_RX_STA |= 0x4000;
						else{
							USART_RX_BUF[USART_RX_STA&0X3FFF] = res;
							USART_RX_STA++; 
							if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;//接收数据错误,重新开始接收	  
						}		 
					}
				} 
				break;  	
			case RECV_BY_TIME_SEPRAITON:
				if(!(USART_RX_STA & 0x8000)){
					USART_RX_BUF[USART_RX_STA & 0X3FFF] = res;
					USART_RX_STA++; 
					if(USART_RX_STA > (USART_REC_LEN - 1)) USART_RX_STA = 0;//接收数据错误,重新开始接收	 
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
			//if(rmode != RECV_OFF) USART->CR1 |= 1 << 5; //RXIE
			//if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
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
			while((USART_RX_STA & 0x8000)==0){//接收未完成
				for(i = 0; i/5000 < timeout && !(USART->SR & (1 << 5)) && !(USART_RX_STA & 0x8000); i++);
				if(i/5000 >= timeout) return 0;
				if(USART_RX_STA & 0x8000) break;
				res = USART->DR;
				LED0 = ~LED0;
				if(USART_RX_STA & 0x4000){//接收到了0x0d
					if(res != 0x0a) USART_RX_STA = 0;//接收错误,重新开始
					else USART_RX_STA |= 0x8000;	//接收完成了 
				}else{ //还没收到0X0D	
					if(res == 0x0d && (rmode != RECV_BY_TIME_SEPRAITON)) USART_RX_STA |= 0x4000;
					else{
						buf[USART_RX_STA&0X3FFF] = res;
						USART_RX_STA++; 
						if(USART_RX_STA > (USART_REC_LEN-1)) USART_RX_STA = 0;//接收数据错误,重新开始接收	  
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
	tc_flag = 0;
	//USART->CR1 &= ~(1 << 4); //IDLE
	if(tx_way == BLOCK){
		while((USART->SR & 0X40) == 0);
		USART->DR = val;   
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		//USART->CR1 &= ~(1 << 5); //RXIE
		tx_len = 1;
		ti = 0;
		USART->DR = val;
	}
	
}

void UART::write(u8 *buf, u32 len){
	tc_flag = 0;
	//USART->CR1 &= ~(1 << 4); //IDLE
	if(tx_way == BLOCK){
		for(u32 i = 0; i < len; i++){
			write(buf[i]);
		}
		tc_flag = 1;
		if(rmode == RECV_BY_TIME_SEPRAITON) USART->CR1 |= 1 << 4; //IDLE
	}else if(tx_way == ITR){
		//USART->CR1 &= ~(1 << 5); //RXIE
		for(u16 i = 0; i < len; i++) tx_buf[i] = buf[i];
		tx_len = len;
		ti = 0;
		USART->SR &= ~(1 << 6);
		USART->CR1 |= 1 << 6; //TCIE
		USART->DR = tx_buf[0];
	}
	
}

u8 UART::write_available(){
	return tc_flag;
}
