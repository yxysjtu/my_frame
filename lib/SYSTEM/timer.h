/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-03 00:35:07
 */
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
    reset_event,
    enable_event,
    update_event,
    cmp_match_event,
    cmp1_match_event,
    cmp2_match_event,
    cmp3_match_event,
    cmp4_match_event
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

typedef struct{ //not include:: CHxN:reverse phase of pwm; BKIN:break in
    pin etr;
    pin ch1;
    pin ch2;
    pin ch3;
    pin ch4;
} TIM_pin;

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
****************************************************************/

class timer{
private:
    void (*update_func)(void);
    void (*trigger_func)(void);
    void (*cc_func[4])(void);
    TIM_TypeDef *tim;

public:
    timer(TIM_TypeDef *tim);

	void init(tim_mode mode=continuous);
    void attach_ITR(void (*f)(void), tim_event event=update, NVIC_priority priority=default_priority_H);
    void IRQHandler(void); 

    void set_direction(u8 dir); //dir 0:up, 1:down
    void set_frequency(u32 f); //for not so accurate application
    void set_frequency(u16 arr, u16 psc);
    void set_channel_mode(TIM_CHx ch, tim_channel_mode mode, u8 pin_remap=0, u8 polarity=0);//ch == ETR not supported, tim5~8 input,output not supported
    void set_pulsewidth(TIM_CHx ch, float pw); //pw:0 ~ 1, first set frequency because pw depends on arr

    void set_master_out(master_out_source m);
    /*ITR | 0 | 1 | 2 | 3
    *-----|---|---|---|---
    *TIM2 | 1 | 8 | 3 | 4
    *TIM3 | 1 | 2 | 5 | 4
    *TIM4 | 1 | 2 | 3 | 8
    *TIM5 | 2 | 3 | 4 | 8
    (slave_in_trigger_mapping)*/
    void set_slave_in(slave_in_source s);
	
	void enable(); 
    void disable();
    void enable_channel(TIM_CHx ch);
    void disable_channel(TIM_CHx ch);

    u16 get_cnt();

};

extern timer tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8;

#endif
