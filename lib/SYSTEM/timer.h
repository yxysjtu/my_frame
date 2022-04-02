/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 19:04:50
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
    TIM_CH1,
    TIM_CH2,
    TIM_CH3,
    TIM_CH4
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
    ETR,
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
    void set_channel_mode(TIM_CHx ch, tim_channel_mode mode, pin p);
    void set_pulsewidth(TIM_CHx ch, float pw); //0 ~ 1

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

    u16 get_cnt();

};

extern timer tim1, tim2, tim3, tim4, tim5, tim6, tim7, tim8;

#endif
