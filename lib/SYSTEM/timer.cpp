/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 13:14:51
 */
#include "timer.h"
#include "debug.h"

timer tim1(TIM1), tim2(TIM2), tim3(TIM3), tim4(TIM4);
#ifdef TIM5
timer tim5(TIM5);
#endif
#ifdef TIM6
timer tim6(TIM6);
#endif
#ifdef TIM7
timer tim7(TIM7);
#endif
#ifdef TIM8
timer tim8(TIM8);
#endif

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
#ifdef TIM5
TIM_pin tim5_pin[1] = { //NO ETR
    {{GPIOA,0},{GPIOA,0},{GPIOA,1},{GPIOA,2},{GPIOA,3}}
};
#endif

timer::timer(TIM_TypeDef *tim){
    this->tim = tim;
	update_func = 0;
	trigger_func = 0;
	for(u8 i = 0; i < 4; i++) cc_func[i] = 0;
}

void timer::init(tim_mode mode){
	//enable clock
    if((u32)tim == (u32)TIM1){
        RCC->APB2ENR |= 1 << 11;
    }
    #ifdef TIM8
    else if((u32)tim == (u32)TIM8){
        RCC->APB2ENR |= 1 << 13;
    }
    #endif
    else{
        u32 timnum = ((u32)tim - TIM2_BASE) / 0x0400;
        RCC->APB1ENR |= 1 << timnum;
    }
    //config
    tim->CR1 |= mode << 3; //OPM
}

void timer::attach_ITR(void (*f)(void), tim_event event, NVIC_priority priority){
    if(event == update) update_func = f;
    else if(event == trigger) trigger_func = f;
    else cc_func[event - 1] = f;

    tim->DIER |= 1 << event; //ITR enable
    
    switch((u32)tim){ //NVIC
        case TIM1_BASE:
            if(event == update) NVIC_init(TIM1_UP_IRQn, priority);
            else if(event == trigger) NVIC_init(TIM1_TRG_COM_IRQn, priority); 
            else NVIC_init(TIM1_CC_IRQn , priority);
            break;
        case TIM2_BASE: NVIC_init(TIM2_IRQn, priority); break;
        case TIM3_BASE: NVIC_init(TIM3_IRQn, priority); break;
        case TIM4_BASE: NVIC_init(TIM4_IRQn, priority); break;
        #ifdef TIM5
        case TIM5_BASE: NVIC_init(TIM5_IRQn, priority); break;
        #endif
        #ifdef TIM6
        case TIM6_BASE: NVIC_init(TIM6_IRQn, priority); break;
        #endif
        #ifdef TIM7
        case TIM7_BASE: NVIC_init(TIM7_IRQn, priority); break;
        #endif
        #ifdef TIM8
        case TIM8_BASE:
            if(event == update) NVIC_init(TIM8_UP_IRQn, priority);
            else if(event == trigger) NVIC_init(TIM8_TRG_COM_IRQn , priority); 
            else NVIC_init(TIM8_CC_IRQn , priority);
            break;
        #endif
        default: break;
    }
    
}
void timer::IRQHandler(void){
	if((u32)tim == TIM5_BASE) debug("irq",0);
    if(tim->SR & (1 << update)){
        if(update_func != 0) update_func();
        tim->SR &= ~(1 << update);
    }else if(tim->SR & (1 << trigger)){
        if(trigger_func != 0) trigger_func();
        tim->SR &= ~(1 << trigger);
    }else{
        for(u8 i = 1; i <= 4; i++){
            if(tim->SR & (1 << i)){
                if(cc_func[i - 1] != 0){
					cc_func[i - 1]();
					debug("func",i);
					tim->SR &= ~(1 << i);
				}
            }
        }
    }
}

void timer::enable(){
    tim->CR1 |= 1 << 0; //enable
}
void timer::disable(){
    tim->CR1 &= ~(1 << 0);
}

void timer::enable_channel(TIM_CHx ch){
    tim->CCER |= 1 << (4 * (ch - 1));
}

void timer::disable_channel(TIM_CHx ch){
    tim->CCER &= ~(1 << (4 * (ch - 1)));
}

void timer::set_direction(u8 dir){
    if(dir) tim->CR1 |= 1 << 4;
    else tim->CR1 &= ~(1 << 4);
}
void timer::set_frequency(u32 f){
    u32 arr = 72000000 / f;
    u32 psc = 1;
    while(arr > 65536){
        arr >>= 1;
        psc <<= 1;
    }
    tim->ARR = arr - 1;
    tim->PSC = psc - 1; 
}
void timer::set_frequency(u16 arr, u16 psc){
    tim->ARR = arr;
    tim->PSC = psc;
}
void timer::set_channel_mode(TIM_CHx ch, tim_channel_mode mode, u8 pin_remap, u8 polarity){
    //pin remap
	if(pin_remap != 0) RCC->APB2ENR |= 1 << 0;
    TIM_pin tim_p;
    switch((u32)tim){ //TODO:tim5~8 input,output not supported
        case TIM1_BASE: {
            tim_p = tim1_pin[pin_remap]; 
            if(pin_remap == 2) pin_remap++; 
			if(pin_remap != 0){
				AFIO->MAPR &= ~((1 << 6) + (1 << 7));
				AFIO->MAPR |= pin_remap << 6;
			}
        } break;
        case TIM2_BASE: {
            tim_p = tim2_pin[pin_remap]; 
			if(pin_remap != 0){
				AFIO->MAPR &= ~((1 << 8) + (1 << 9));
				AFIO->MAPR |= pin_remap << 8;
			}
        } break;
        case TIM3_BASE: {
            tim_p = tim3_pin[pin_remap]; 
            if(pin_remap >= 1) pin_remap++; 
			if(pin_remap != 0){
				AFIO->MAPR &= ~((1 << 10) + (1 << 11));
				AFIO->MAPR |= pin_remap << 10;
			}
        } break;
        case TIM4_BASE: {
            tim_p = tim4_pin[pin_remap]; 
			if(pin_remap != 0){
				AFIO->MAPR &= ~(1 << 12);
				AFIO->MAPR |= pin_remap << 12;
			}
        } break;
        #ifdef TIM5
        case TIM5_BASE: {
            tim_p = tim5_pin[0]; 
        } break;
        #endif
        default: break;
    }
    //set channel config
    switch (mode){
        case pwm:{
			switch(ch){
				case TIM_CH1:{
					pinMode(tim_p.ch1, ALTERNATE_OUTPUT);
                    tim->CCMR1 &= ~(3 << 0);//output
					tim->CCMR1 |= 6 << 4; //pwm mode, 110 start from 1, 111 start from 0
					tim->CCMR1 |= 1 << 3; //auto load buf enable
				} break;
				case TIM_CH2:{
					pinMode(tim_p.ch2, ALTERNATE_OUTPUT);
                    tim->CCMR1 &= ~(3 << 8);//output
					tim->CCMR1 |= 6 << 12; //pwm mode, 110 start from 1, 111 start from 0
					tim->CCMR1 |= 1 << 11; //auto load buf enable
				} break;
				case TIM_CH3:{
					pinMode(tim_p.ch3, ALTERNATE_OUTPUT);
                    tim->CCMR2 &= ~(3 << 0);//output
					tim->CCMR2 |= 6 << 4; //pwm mode, 110 start from 1, 111 start from 0
					tim->CCMR2 |= 1 << 3; //auto load buf enable
				} break;
				case TIM_CH4:{
					pinMode(tim_p.ch4, ALTERNATE_OUTPUT);
                    tim->CCMR2 &= ~(3 << 8);//output
					tim->CCMR2 |= 6 << 12; //pwm mode, 110 start from 1, 111 start from 0
					tim->CCMR2 |= 1 << 11; //auto load buf enable
				} break;
				default: break;
			}
            if((u32)tim == TIM1_BASE) tim->BDTR |= 1 << 15; //MOE
        } break;
        case input_capture:{
            switch(ch){
				case TIM_CH1:{
					pinMode(tim_p.ch1, INPUT);
                    tim->CCMR1 &= ~(3 << 0);
                    tim->CCMR1 |= 1 << 0; //input
				} break;
				case TIM_CH2:{
					pinMode(tim_p.ch2, INPUT);
                    tim->CCMR1 &= ~(3 << 8);
                    tim->CCMR1 |= 1 << 8; //input
				} break;
				case TIM_CH3:{
					pinMode(tim_p.ch3, INPUT);
                    tim->CCMR2 &= ~(3 << 0);
                    tim->CCMR2 |= 1 << 0; //input
				} break;
				case TIM_CH4:{
					pinMode(tim_p.ch4, INPUT);
                    tim->CCMR2 &= ~(3 << 8);
                    tim->CCMR2 |= 1 << 8; //input
				} break;
				default: break;
			}
        } break;
        case output_compare:{

        } break;
        default: break;
    }
    if(polarity) tim->CCER |= 1 << (4 * (ch - 1) + 1);
}
void timer::set_polarity(TIM_CHx ch, u8 polarity){
    if(polarity) tim->CCER |= 1 << (4 * (ch - 1) + 1);
	else tim->CCER &= ~(1 << (4 * (ch - 1) + 1));
}
void timer::set_input_filter(TIM_CHx ch, tim_input_filter input_filter, u8 input_prescaler){
    if(ch == TIM_CH1 || ch == TIM_CH2){
        tim->CCMR1 &= ~(0x00003f << (2 + 8 * (ch - TIM_CH1)));
        tim->CCMR1 |= (input_prescaler << (2 + 8 * (ch - TIM_CH1))) + (input_filter << (4 + 8 * (ch - TIM_CH1)));
    }else if(ch == TIM_CH3 || ch == TIM_CH4){
        tim->CCMR2 &= ~(0x00003f << (2 + 8 * (ch - TIM_CH3)));
        tim->CCMR2 |= (input_prescaler << (2 + 8 * (ch - TIM_CH3))) + (input_filter << (4 + 8 * (ch - TIM_CH3)));
    }
}
void timer::set_pulsewidth(TIM_CHx ch, float pw){ //first set frequency
    switch (ch){
        case TIM_CH1: tim->CCR1 = (u32)(pw * tim->ARR); break;
        case TIM_CH2: tim->CCR2 = (u32)(pw * tim->ARR); break;
        case TIM_CH3: tim->CCR3 = (u32)(pw * tim->ARR); break;
        case TIM_CH4: tim->CCR4 = (u32)(pw * tim->ARR); break;
        default: break;
    }
}

void timer::set_master_out(master_out_source m){
    tim->CR2 = 0;
    tim->CR2 |= (m << 4);
}

void timer::set_slave_in(slave_in_source s){

}

u16 timer::get_cnt(){
    return tim->CNT;
}

u16 timer::get_ccr(TIM_CHx ch){
	switch(ch){
		case TIM_CH1: return tim->CCR1;
		case TIM_CH2: return tim->CCR2;
		case TIM_CH3: return tim->CCR3;
		case TIM_CH4: return tim->CCR4;
		default: return 0;
	}
}

//you need also set dma channel
//reference: p148
void timer::enable_dma_request(tim_event event) {
    tim->DIER |= 1 << (event + 8);
    tim->DIER |= 1 << 14; //enable dma request
}

#ifdef __cplusplus
extern "C" {
#endif
	
void TIM1_UP_IRQHandler(void){
	tim1.IRQHandler();
}
void TIM1_TRG_COM_IRQHandler(void){
	tim1.IRQHandler();
}
void TIM1_CC_IRQHandler(void){
	tim1.IRQHandler();
}

void TIM2_IRQHandler(void){
	tim2.IRQHandler();
}

void TIM3_IRQHandler(void){
	tim3.IRQHandler();
}

void TIM4_IRQHandler(void){
	tim4.IRQHandler();
}

void TIM5_IRQHandler(void){
	tim5.IRQHandler();
}

void TIM6_IRQHandler(void){
	tim6.IRQHandler();
}

void TIM7_IRQHandler(void){
	tim7.IRQHandler();
}

void TIM8_UP_IRQHandler(void){
	tim8.IRQHandler();
}
void TIM8_TRG_COM_IRQHandler(void){
	tim8.IRQHandler();
}
void TIM8_CC_IRQHandler(void){
	tim8.IRQHandler();
}

#ifdef __cplusplus
}
#endif
