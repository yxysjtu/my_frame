/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-05 10:30:44
 */
 
 /*****************pin mapping reference*******************
 *    ETR     |    CH1   |   CH2   |    CH3   |    CH4
TIM_pin tim1_pin[2] = {
    {{GPIOA,12},{GPIOA,8},{GPIOA,9},{GPIOA,10},{GPIOA,11}},
    {{GPIOE,7},{GPIOE,9},{GPIOE,11},{GPIOE,13},{GPIOE,14}}
};

TIM_pin tim2_pin[4] = {
    {{GPIOA,0},{GPIOA,0},{GPIOA,1},{GPIOA,2},{GPIOA,3}},
    {{GPIOA,15},{GPIOA,15},{GPIOB,3},{GPIOA,2},{GPIOA,3}},
    {{GPIOA,0},{GPIOA,0},{GPIOA,1},{GPIOB,10},{GPIOB,11}},
    {{GPIOA,15},{GPIOA,15},{GPIOB,3},{GPIOB,10},{GPIOB,11}}
};

TIM_pin tim3_pin[3] = {
    {{GPIOD,2},{GPIOA,6},{GPIOA,7},{GPIOB,0},{GPIOB,1}},
    {{GPIOD,2},{GPIOB,4},{GPIOB,5},{GPIOB,0},{GPIOB,1}},
    {{GPIOD,2},{GPIOC,6},{GPIOC,7},{GPIOC,8},{GPIOC,9}}
};

TIM_pin tim4_pin[2] = {
    {{GPIOE,0},{GPIOB,6},{GPIOB,7},{GPIOB,8},{GPIOB,9}},
    {{GPIOE,0},{GPIOD,12},{GPIOD,13},{GPIOD,14},{GPIOD,15}}
};
TIM_pin tim5_pin[1] = { //NO ETR
    {{GPIOA,0},{GPIOA,0},{GPIOA,1},{GPIOA,2},{GPIOA,3}}
};
****************************************************************/

 /****************example code***********************
 1.counter-interrupt
	void led1_flip(){
		LED1 = ~LED1;
	}
	
	main:
	tim2.init();
	tim2.set_frequency(1);
	tim2.attach_ITR(led1_flip);
	tim2.enable();
	
 2.pwm
	tim3.init();
	tim3.set_frequency(500); //freq higher, resolution lower
	tim3.set_channel_mode(TIM_CH2, pwm, 1); //remap 1
	tim3.set_pulsewidth(TIM_CH2, 0.5);
	tim3.enable_channel(TIM_CH2);
	tim3.enable();
	
	while(1){
		for(float i = 1; i >= 0; i -= 0.01){
			tim3.set_pulsewidth(TIM_CH2, i);
			delay(10);
		}
		for(float i = 0; i <= 1; i += 0.01){
			tim3.set_pulsewidth(TIM_CH2, i);
			delay(10);
		}
	}
	
 3.input-capture
	void edge_detected(){
		if(!polarity){
			polarity = 1;
			tim5.set_polarity(TIM_CH1, polarity);
			cnt = tim5.get_ccr(TIM_CH1);
			uart1.printf("press\r\n", cnt);
		}else{
			polarity = 0;
			tim5.set_polarity(TIM_CH1, polarity);
			cnt += tim5.get_ccr(TIM_CH1);
			uart1.printf("pulse:%dms\r\n", cnt / 1000);
		}
	}
	void cnt_overflow(){
		cnt += 65536;
	}
	
	main:
	tim5.init();
	tim5.set_frequency(65535, 71);
	tim5.set_channel_mode(TIM_CH1, input_capture);
	tim5.attach_ITR(edge_detected, capture_compare1);
	tim5.attach_ITR(cnt_overflow, update);
	tim5.enable_channel(TIM_CH1);
	tim5.enable();


 ****************************************************/
 
#ifndef TIMER_H
#define TIMER_H

#include "basic.h"

typedef enum{
    continuous = 0,
    one_pulse = 1
} tim_mode;

typedef enum{
    pwm,
    input_capture,
    output_compare
} tim_channel_mode;

typedef enum{
    ETR = 0,
    TIM_CH1 = 1,
    TIM_CH2 = 2,
    TIM_CH3 = 3,
    TIM_CH4 = 4
} TIM_CHx;

typedef enum{
    reset_event = 0,
    enable_event = 1,
    update_event = 2,
    cmp_match_event = 3,
    cmp1_match_event = 4,
    cmp2_match_event = 5,
    cmp3_match_event = 6,
    cmp4_match_event = 7
} master_out_source;

typedef enum{
    ITR0,
    ITR1,
    ITR2,
    ITR3,
    ETR1,
    TI1,
    TIF1, //filtered
    TIF2
} slave_in_source;

typedef enum{
    update = 0,
    trigger = 6,
    capture_compare1 = 1,
    capture_compare2 = 2,
    capture_compare3 = 3,
    capture_compare4 = 4
} tim_event;

typedef enum{
    fin_div1_n1 = 0,
    fclk_div1_n2 = 1,
    fclk_div1_n4 = 2,
    fclk_div1_n8 = 3,
    fin_div2_n6 = 4,
    fin_div2_n8 = 5,
    fin_div4_n6 = 6,
    fin_div4_n8 = 7,
    fin_div8_n6 = 8,
    fin_div8_n8 = 9,
    fin_div16_n5 = 10,
    fin_div16_n6 = 11,
    fin_div16_n8 = 12,
    fin_div32_n5 = 13,
    fin_div32_n6 = 14,
    fin_div32_n8 = 15,
} tim_input_filter;

typedef struct{ //not include:: CHxN:reverse phase of pwm; BKIN:break in
    pin etr;
    pin ch1;
    pin ch2;
    pin ch3;
    pin ch4;
} TIM_pin;


class timer{
private:
    void (*update_func)(void);
    void (*trigger_func)(void);
    void (*cc_func[4])(void);
public:
	TIM_TypeDef *tim;
	
    timer(TIM_TypeDef *tim);

	void init(tim_mode mode=continuous);
    void attach_ITR(void (*f)(void), tim_event event=update, NVIC_priority priority=default_priority_H); 
	void IRQHandler(void);
	void enable_dma_request(tim_event event=update);

    void set_direction(u8 dir); //dir 0:up, 1:down
    void set_frequency(u32 f); //for not so accurate application
    void set_frequency(u16 arr, u16 psc);
    void set_channel_mode(TIM_CHx ch, tim_channel_mode mode, u8 pin_remap=0, u8 polarity=0);//ch == ETR not supported; tim5~8 input,output not supported
    void set_input_filter(TIM_CHx ch, tim_input_filter input_filter, u8 input_prescaler=0); //input_prescaler:0:n1;1:n2;2:n4;3:n8
    void set_polarity(TIM_CHx ch, u8 polarity); //0:none-inverted, output active high, input rising edge; 1:inverted, output active low, input falling edge; 
    void set_pulsewidth(TIM_CHx ch, float pw); //pw:0 ~ 1, first set frequency because pw depends on arr

    void set_master_out(master_out_source m);
    /*ITR | 0 | 1 | 2 | 3
    *-----|---|---|---|---
    *TIM2 | 1 | 8 | 3 | 4
    *TIM3 | 1 | 2 | 5 | 4
    *TIM4 | 1 | 2 | 3 | 8
    *TIM5 | 2 | 3 | 4 | 8
    (slave_in_trigger_mapping)*/
    void set_slave_in(slave_in_source s);//TODO
	
	void enable(); 
    void disable();
    void enable_channel(TIM_CHx ch);
    void disable_channel(TIM_CHx ch);

    u16 get_cnt();
	u16 get_ccr(TIM_CHx ch);

};

extern timer tim1, tim2, tim3, tim4;
#ifdef TIM5
extern timer tim5;
#endif
#ifdef TIM6
extern timer tim6;
#endif
#ifdef TIM7
extern timer tim7;
#endif
#ifdef TIM8
extern timer tim8;
#endif

#endif
