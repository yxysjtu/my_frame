#include "adc.h"

ADC adc1(ADC1), adc2(ADC2);
#ifdef ADC3
ADC adc3(ADC3);
#endif

ADC::ADC(ADC_TypeDef* adc){
    this->adc = adc;
    func = 0;
}

void ADC::init(u8 continuous, u8 dma_en){
    u8 adcn;
    if(adc == ADC1) adcn = 9;
    else if(adc == ADC2) adcn = 10;
    else adcn = 15;

    RCC->APB2ENR |= 1 << adcn;
    RCC->APB2RSTR |= 1 << adcn;
    RCC->APB2RSTR &= ~(1 << adcn);
    RCC->CFGR &= ~(3 << 14);
    RCC->CFGR |= 2 << 14; //12M

    adc->CR1 &= ~(0xf << 16); //independent mode
    adc->CR1 &= ~(1 << 8); //non-scan mode
    adc->CR2 |= (continuous << 1) + (dma_en << 8) + (7 << 17) + (1 << 20); //SWSTART, EXTrigger
    adc->CR2 &= ~(1 << 11); //right align
    adc->SQR1 &= ~(0xf << 20); //1 channel conversion
    /*adc->SMPR2 &= ~(7 << 0);
    adc->SMPR2 |= PERIOD_41 << */
    adc->CR2 |= 1 << 0; //ad on
    adc->CR2 |= 1 << 3; //reset callibration
    while(adc->CR2 & (1 << 3)); //wait for calli reset done
    adc->CR2 |= 1 << 2;
    while(adc->CR2 & (1 << 2)); //wait for calli done

}

void ADC::config_channel(u8 ch, pin p, adc_sample_t t){
    pinMode(p, ANALOG_INPUT);
    if(ch <= 9){
        adc->SMPR2 &= ~(7 << (3 * ch));
        adc->SMPR2 |= t << (3 * ch);
    }else{
        ch -= 10;
        adc->SMPR1 &= ~(7 << (3 * ch));
        adc->SMPR1 |= t << (3 * ch);
    }
}
void ADC::add_scan_channel(u8 ch, pin p, adc_sample_t t){
    config_channel(ch, p, t);
    adc->CR1 |= 1 << 8; //scan mode
    u8 n = (adc->SQR1 & (0xf << 20)) >> 20;
    if(n <= 5){
        adc->SQR3 &= ~(31 << (5 * n));
        adc->SQR3 |= ch << (5 * n);
    }else if(n <= 11){
        adc->SQR2 &= ~(31 << (5 * (n - 6)));
        adc->SQR2 |= ch << (5 * (n - 6));
    }else{
        adc->SQR1 &= ~(31 << (5 * (n - 12)));
        adc->SQR1 |= ch << (5 * (n - 12));
    }
    adc->SQR1 &= ~(0xf << 20);
    adc->SQR1 |= (u32)(n + 1) << 20;
}
void ADC::attach_ITR(void (*f)(void), NVIC_priority priority){
    func = f;
    adc->CR1 |= 1 << 5; //EOCIE
    if(adc == ADC1 || adc == ADC2){
        NVIC_init(ADC1_2_IRQn, priority);
    }else{
        #ifdef ADC3
        NVIC_init(ADC3_IRQn, priority);
        #endif
    }
}
void ADC::IRQHandler(){
    if(adc->SR & (1 << 1)){
        if(func != 0) func();
        adc->SR &= ~(1 << 1);
    }
}

float ADC::read(u8 ch){
    adc->SQR3 &= ~(7 << 0);
    adc->SQR3 |= ch;
    adc->CR2 |= 1 << 22; //SWSTART
    while(!(adc->SR & (1 << 1))); //wait for conversion done
    return (float)adc->DR / 4096 * 3.3;
}
float ADC::read_aver(u8 ch, u8 n){
    adc->SQR3 &= ~(7 << 0);
    adc->SQR3 |= ch;
    
    u32 temp = 0;
    for(int i = 0; i < n; i++){
        adc->CR2 |= 1 << 22; //SWSTART
        while(!(adc->SR & (1 << 1))); //wait for conversion done
        temp += adc->DR;
    }
    temp /= n;
    return (float)temp / 4096 * 3.3;
}

float ADC::value(){
    return (float)adc->DR / 4096 * 3.3;
}


#ifdef __cplusplus
extern "C" {
#endif
	
void ADC1_2_IRQHandler(void){
	adc1.IRQHandler();
	adc2.IRQHandler();
}
#ifdef ADC3
void ADC3_IRQHandler(void){
	adc3.IRQHandler();
}
#endif
	
#ifdef __cplusplus
}
#endif
