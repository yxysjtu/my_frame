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
#include "dma.h"

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
	DMA //no dma in uart4 uart5
} io_t;

typedef struct {
	pin rx;
	pin tx;
} UART_pin;

/****pin mapping reference*******
 *      RX    |      TX
UART_pin uart1_pin[2] = {
	{{GPIOA, 10},{GPIOA, 9}},
	{{GPIOB, 7}, {GPIOB, 6}}
};
UART_pin uart2_pin[2] = { 
	{{GPIOA, 3},{GPIOA, 2}},
	{{GPIOD, 6},{GPIOD, 5}}
};
UART_pin uart3_pin[3] = { 
	{{GPIOB, 11},{GPIOB, 10}},
	{{GPIOC, 11},{GPIOC, 10}},
	{{GPIOD, 9}, {GPIOD, 8}}
};
**********************************/

class UART {
public:
	u8 USART_RX_BUF[USART_REC_LEN]; //user can use this buf when RECV_CUSTOMIZE
	u8 tx_buf[USART_REC_LEN];
	DMA_CH *rdma, *tdma;
	vu8 tc_flag; //transmitt complete
	vu16 USART_RX_STA; //bit15:rc_flag; bit14:\r recv; bit0~13:rx_len

	UART(USART_TypeDef *uart=USART1);
	//recommend:
	//RECV_BY_TIME_SEPRAITON + rx:DMA
	//RECV_BY_LINE_ENDING + rx:ITR
	void enable(u8 pin_remap=0, recv_mode rmode=RECV_BY_TIME_SEPRAITON, io_t rx_way=ITR, io_t tx_way=BLOCK, u32 baudrate=115200, parity_e parity=parity_none);
	void disable();
	
	u8 IRQHandler(void);

	//RECV_BY_LINE: \r\n(support block,itr)
	//RECV_BY_TIME_SEPRATION: data packet(support block,itr,dma)
	//RECV_IN_CIRCULAR_BUF: all data in buf(block not supported)
	u8 readline(u8 *buf, u8 *len, u32 timeout=1000); //return read success=1, fail=0
	u8 read(u8 *buf, u16 len);//circular buf, return read success=1, fail=0
	void printf(const char *fmt,...);
	void write(u8 val);
	void write(u8 *buf, u32 len);
	u8 write_available();
	
private:
	recv_mode rmode;
	io_t rx_way, tx_way;
	USART_TypeDef * USART; 

	vu16 ri1, ri2, rovf; //recv_buf top, button pointer; overflow flag
	vu8 ti;
	vu16 tx_len;
};

extern UART uart1, uart2, uart3;
#ifdef UART4
extern UART uart4;
#endif
#ifdef UART5
extern UART uart5;
#endif

#endif	   

