#ifndef __USART_H
#define __USART_H

#ifdef __cplusplus
extern "C" {
#endif
	
#include <stdio.h>	
#include <stdarg.h>

#ifdef __cplusplus
}
#endif

#include "basic.h"

#define USART_REC_LEN	200


typedef enum {
	RECV_OFF = 0,
	RECV_BY_LINE_ENDING = 1, //\r\n, linear buf, if received ending then stop put recv data into buf until exec readline function
	RECV_BY_TIME_SEPRAITON = 2, 
	RECV_IN_CIRCULAR_BUF = 3, //if full then lose data
	RECV_CUSTOMIZE = 4
} recv_mode;

typedef enum{
	parity_none = 0,
	parity_odd = 3,
	parity_even = 2
} parity_e;

typedef enum{
	BLOCK,
	ITR,
	DMA
} io_t;

typedef struct {
	pin rx;
	pin tx;
} UART_pin;

extern UART_pin default_uart_pin;

class UART { //do not use ITR of rx and tx at the same time!!!
	public:
		u8 USART_RX_BUF[USART_REC_LEN]; //user can use this buf when RECV_CUSTOMIZE
		u8 tx_buf[USART_REC_LEN];
	
		UART(recv_mode rmode=RECV_OFF, io_t rx_way=ITR, io_t tx_way=BLOCK, USART_TypeDef * uart=USART1, UART_pin p=default_uart_pin);
		
		void begin(u32 baudrate=115200, parity_e parity=parity_none);
		void printf(const char *fmt,...);
	
		//使用：（一定要写）在外部写USARTx_IRQHandler, 在函数里调用uartx.IRQHandler
		//返回接收到的单个值
		u8 IRQHandler(void);

		//RECV_BY_LINE: \r\n(support block,itr,dma)
		//RECV_BY_TIME_SEPRATION: data packet(support block,itr,dma)
		//RECV_IN_CIRCULAR_BUF: all data in buf(block not supported)
		u8 readline(u8 *buf, u8 *len, u32 timeout=1000); //return read success=1, fail=0
		u8 read(u8 *buf, u16 len);//circular buf, return read success=1, fail=0
		void write(u8 val);
		void write(u8 *buf, u32 len);
		u8 write_available();
	
	private:
		UART_pin p;
		recv_mode rmode;
		io_t rx_way, tx_way;
		USART_TypeDef * USART; 
		//接收状态
		//bit15，	接收完成标志
		//bit14，	接收到0x0d
		//bit13~0，	接收到的有效字节数目
		vu16 USART_RX_STA;         		//接收状态标记	
		vu16 ri1, ri2, rovf; //recv_buf top, button pointer; overflow flag
		vu8 ti;
		vu16 tx_len;
		vu8 tc_flag; //transmitt complete
	
};

#endif	   
















