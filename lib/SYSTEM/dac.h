//
// Created by asus on 2022/7/1.
//

#ifndef MYFRAME_DAC_H
#define MYFRAME_DAC_H

#include "basic.h"
#include "timer.h"
#include "dma.h"

typedef enum{
    tim6_trgo = 0,
    tim8_trgo = 1,
    tim7_trgo = 2,
    tim5_trgo = 3,
    tim2_trgo = 4,
    tim4_trgo = 5,
    exti9 = 6,
    software = 7
} trig_sel;

class DAC_CH{
private:
    u8 dacn;

public:
    DAC_CH(u8 dacn);

    void init();

    void set_waveform(u16* buf, u32 len, u32 freq, timer& tim);

    void write(u16 val); //0~4095

    void set_trigger(trig_sel tsel);
};

extern DAC_CH dac1, dac2;

#endif //MYFRAME_DAC_H
