/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-05 00:35:16
 */
#include "pulse.h"

u8 pulsein_polarity[4]; //a lot waste of memory.... I'm lazy
u8 pulsein_polarity0[4];
u8 detect_flag[4];
u32 pulsein_cnt[4];
u8 pulseout_flag[4] = {0};
int pulseout_cnt[4];

TIM_TypeDef *pulseintim, *pulseouttim;

void set_pulsein_polarity(u8 ch, u8 polarity){
    if(polarity) pulseintim->CCER |= 1 << (4 * (ch - 1) + 1);
	else pulseintim->CCER &= ~(1 << (4 * (ch - 1) + 1));
}

void pulsein_cnt_overflow(){
    for(u8 i = 0; i < 4; i++) pulsein_cnt[i] += 65536;
}
void edge_detected1(){
    u8 i = 0;
    if(pulsein_polarity[i] == pulsein_polarity0[i]){ //start
        pulsein_polarity[i] = !pulsein_polarity[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] = pulseintim->CCR1;
        detect_flag[i] = 0;
    }else{ //end
        pulsein_polarity[i] = pulsein_polarity0[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] += pulseintim->CCR1;
        detect_flag[i] = 1;
    }
}
void edge_detected2(){
    u8 i = 1;
    if(pulsein_polarity[i] == pulsein_polarity0[i]){ //start
        pulsein_polarity[i] = !pulsein_polarity[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] = pulseintim->CCR2;
        detect_flag[i] = 0;
    }else{ //end
        pulsein_polarity[i] = pulsein_polarity0[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] += pulseintim->CCR2;
        detect_flag[i] = 1;
    }
}
void edge_detected3(){
    u8 i = 2;
    if(pulsein_polarity[i] == pulsein_polarity0[i]){ //start
        pulsein_polarity[i] = !pulsein_polarity[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] = pulseintim->CCR3;
        detect_flag[i] = 0;
    }else{ //end
        pulsein_polarity[i] = pulsein_polarity0[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] += pulseintim->CCR3;
        detect_flag[i] = 1;
    }
}
void edge_detected4(){
    u8 i = 3;
    if(pulsein_polarity[i] == pulsein_polarity0[i]){ //start
        pulsein_polarity[i] = !pulsein_polarity[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] = pulseintim->CCR4;
        detect_flag[i] = 0;
    }else{ //end
        pulsein_polarity[i] = pulsein_polarity0[i];
        set_pulsein_polarity(i + 1, pulsein_polarity[i]);
        pulsein_cnt[i] += pulseintim->CCR4;
        detect_flag[i] = 1;
    }
}


void pulsein_init(timer &tim, u8 enable_channel, u8 remap, u8 polarity){
    pulseintim = tim.tim;
    for(u8 i = 0; i < 4; i++){
        pulsein_polarity[i] = polarity & (1 << i);
        pulsein_polarity0[i] = polarity & (1 << i);
    }
    tim.init();
	tim.set_frequency(65535, 71);
    if(enable_channel & (1 << 0)){
        tim.set_channel_mode((TIM_CHx)1, input_capture, remap, pulsein_polarity0[0]);
        tim.attach_ITR(edge_detected1, capture_compare1);
        tim.enable_channel((TIM_CHx)1);
    }
    if(enable_channel & (1 << 1)){
        tim.set_channel_mode((TIM_CHx)2, input_capture, remap, pulsein_polarity0[1]);
        tim.attach_ITR(edge_detected2, capture_compare2);
        tim.enable_channel((TIM_CHx)2);
    }
    if(enable_channel & (1 << 2)){
        tim.set_channel_mode((TIM_CHx)3, input_capture, remap, pulsein_polarity0[2]);
        tim.attach_ITR(edge_detected3, capture_compare3);
        tim.enable_channel((TIM_CHx)3);
    }
    if(enable_channel & (1 << 3)){
        tim.set_channel_mode((TIM_CHx)4, input_capture, remap, pulsein_polarity0[3]);
        tim.attach_ITR(edge_detected4, capture_compare4);
        tim.enable_channel((TIM_CHx)4);
    }
    tim.attach_ITR(pulsein_cnt_overflow, update);
	tim.enable();
}

u32 pulsein(u8 channel){
    u8 i = channel - 1;
    if(!detect_flag[i]) return 0;
    detect_flag[i] = 0;
    return pulsein_cnt[i];
}


void pulseout_overflow(){
    for(u8 i = 0; i < 4; i++){
        if(pulseout_flag[i]){
            pulseout_flag[i] = 0;
            pulseouttim->CCER |= 1 << (4 * i);
        }else if(pulseout_cnt[i] > 0){
            pulseout_cnt[i]--;
            if(pulseout_cnt[i] == 0) pulseouttim->CCER &= ~(1 << (4 * i));
        }
    }
}

void pulseout_init(timer &tim, u8 enable_channel, u32 freq, u8 remap, u8 polarity){
    pulseouttim = tim.tim;
    tim.init();
    tim.set_frequency(freq);
    for(u8 i = 1; i < 5; i++){
        if(enable_channel & (1 << (i - 1))){
            tim.set_channel_mode((TIM_CHx)i, pwm, remap, (polarity & (1 << (i - 1))));
        }
    }
    tim.attach_ITR(pulseout_overflow, update);
    tim.enable();
}

void pulseout(u8 channel, float duty, u32 num){
    pulseout_cnt[channel - 1] = num;
    switch(channel){
        case 1: pulseouttim->CCR1 = (u32)(duty * pulseouttim->ARR); break;
        case 2: pulseouttim->CCR2 = (u32)(duty * pulseouttim->ARR); break;
        case 3: pulseouttim->CCR3 = (u32)(duty * pulseouttim->ARR); break;
        case 4: pulseouttim->CCR4 = (u32)(duty * pulseouttim->ARR); break;
        default: break;
    }  
    pulseout_flag[channel - 1] = 1;
}
