#ifndef ADC_H
#define ADC_H

#include "basic.h"

typedef enum{ // div t/12 us
    PERIOD_14 = 0,
    PERIOD_20 = 1,
    PERIOD_26 = 2,
    PERIOD_41 = 3,
    PERIOD_54 = 4,
    PERIOD_68 = 5,
    PERIOD_84 = 6,
    PERIOD_252 = 7
} adc_sample_t;

class ADC{
private:
    ADC_TypeDef* adc;
    void (*func)(void);
public:
    ADC(ADC_TypeDef* adc);

    void init(u8 continuous=0, u8 dma_en=0);
    void attach_ITR(void (*f)(void), NVIC_priority priority=default_priority_H);
    void config_channel(u8 ch, pin p, adc_sample_t t);
    void add_scan_channel(u8 ch, pin p, adc_sample_t t);

    //single
    float read(u8 ch);
    float read_aver(u8 ch, u8 n);

    float value();

    void IRQHandler();
};

extern ADC adc1, adc2;
#ifdef ADC3
extern ADC adc3;
#endif

#endif
