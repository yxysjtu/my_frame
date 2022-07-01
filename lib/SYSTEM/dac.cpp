//
// Created by asus on 2022/7/1.
//

#include "dac.h"

DAC_CH dac1(0);
DAC_CH dac2(1);

DAC_CH::DAC_CH(u8 dacn) {
    this->dacn = dacn;
}

void DAC_CH::init() {
    if(dacn == 0) pinMode({GPIOA, 4}, ANALOG_INPUT);
    else pinMode({GPIOA, 5}, ANALOG_INPUT);

    RCC->APB1ENR |= 1 << 29; //dac clock enable

    DAC->CR |= 1 << (1 + 16 * dacn); //DAC1 输出缓存不使能 BOFF1=1
    DAC->CR |= 0 << (6 + 16 * dacn); //不使用波形发生
    DAC->CR |= 0 << (8 + 16 * dacn); //屏蔽、幅值设置

    if(dacn == 0) DAC->DHR12R1 = 0;
    else DAC->DHR12R2 = 0;

    DAC->CR |= 1 << (0 + 16 * dacn); //使能 DAC1

}

void DAC_CH::write(u16 val) { //12 bits
    if(dacn == 0) DAC->DHR12R1 = val;
    else DAC->DHR12R2 = val;
}

void DAC_CH::set_waveform(u16 *buf, u32 len, u32 freq, timer &tim) {
    DAC->CR |= 1 << (12 + 16 * dacn); //dma request enable
    if(dacn){
        dma2_4.init((u32)&(DAC->DHR12R2), (u32)buf, m2p, len, 0, 1, bit_16, bit_16, 0, 1, priority_high);
        dma2_4.enable();
    }else{
        dma2_3.init((u32)&(DAC->DHR12R1), (u32)buf, m2p, len, 0, 1, bit_16, bit_16, 0, 1, priority_high);
        dma2_3.enable();
    }

    tim.init();
    tim.set_frequency(freq * len);
    //tim.enable_dma_request(update);
    tim.set_master_out(update_event);
    if(&tim == &tim2) set_trigger(tim2_trgo);
    else if(&tim == &tim4) set_trigger(tim4_trgo);
#ifdef TIM5
    else if(&tim == &tim5) set_trigger(tim5_trgo);
#endif
#ifdef TIM6
    else if(&tim == &tim6) set_trigger(tim6_trgo);
#endif
#ifdef TIM7
    else if(&tim == &tim7) set_trigger(tim7_trgo);
#endif
#ifdef TIM8
    else if(&tim == &tim8) set_trigger(tim8_trgo);
#endif

    tim.enable();

}

void DAC_CH::set_trigger(trig_sel tsel) {
    DAC->CR |= 1 << (2 + 16 * dacn); //触发功能TEN
    DAC->CR |= tsel << (3 + 16 * dacn);
}

