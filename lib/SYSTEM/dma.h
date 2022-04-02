/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 14:28:19
 */
#ifndef DMA_H
#define DMA_H
	
#include "basic.h"

typedef enum{
    p2m = 0,
    m2p = 1,
} dma_dir;

typedef enum{
    bit_8 = 0,
    bit_16 = 1,
    bit_32 = 2
} data_size;

typedef enum{
    priority_low = 0,
    priority_medium = 1,
    priority_high = 2,
    priority_highhigh = 3
} dma_priority;

class DMA_CH{
private:
	void (*func)(void);
public:
	u16 data_len;
	u8 tc_flag;
	DMA_Channel_TypeDef* DMA_CHx;

    DMA_CH(DMA_Channel_TypeDef* DMA_CHx);
    
    void init(u32 paddr, 
        u32 maddr, 
        dma_dir dir, 
        u16 data_len, 
        u8 pinc, 
        u8 minc, 
        data_size psize=bit_8, 
        data_size msize=bit_8,
        u8 m2m=0,
		u8 circ=0,
        dma_priority priority=priority_medium);
	
	void attach_ITR(void (*f)(void), NVIC_priority priority=default_priority);
	void config(u32 paddr, u32 maddr, u16 data_len);
    void set_data_len(u16 len);
    
    void enable();
	void enable(u16 len);
	u8 transmitt_complete();
    void disable(); //auto reload counter
	
	void IRQHandler(void);//must be wrapped in outside void DMAx_Channely_IRQHandler(void)
};

extern DMA_CH dma1_1, dma1_2, dma1_3, dma1_4, dma1_5, dma1_6, dma1_7;
#ifdef DMA2
extern DMA_CH dma2_1, dma2_2, dma2_3, dma2_4, dma2_5;
#endif

#endif
