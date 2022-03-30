/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 20:11:15
 */
#include "wdg.h"

void iwdg_init(float tout_ms){ //0.1 ~ 26000 ms
    u8 pr = 0;
    u16 rlr;
    float t = tout_ms / 409.6;
    while(t > 1 && pr < 7){
        t /= 2;
        pr++;
    }
    rlr = (u16)(4095 * t);

    IWDG->KR = 0x5555; //cancel protection
    IWDG->PR = pr; //set time_out
    IWDG->RLR = rlr;
    IWDG->KR = 0xAAAA; //reload
    IWDG->KR = 0xCCCC; //start
}

void iwdg_feed(){
    IWDG->KR = 0xAAAA; //reload
}


u8 wwdg_cnt = 0x7f;

void wwdg_init(float tmin_ms, float tmax_ms, u8 EWI_enable){ //0.113ms ~ 58.25ms
    RCC->APB1ENR|=1<<11;

    u8 fpclk = sys_clock * sys_clock_pll / 2;
    u8 wgdtb = 0, w;
    float t = (float)tmax_ms * fpclk * 1000 / 4096;
	while(t > 64){
		t /= 2;
		wgdtb += 1;
	}
	if(wgdtb > 3) wgdtb = 3;
	
    w = (u8)(t + 63 - t * tmin_ms / tmax_ms);
	wwdg_cnt = 0x7f & (u8)(t + 63);
	if(w < 0x40) w = 0x40;
	else if(w >= 0x7f) w = 0x7e;
	
    WWDG->CFR = (wgdtb << 7) + w;
	WWDG->CR = wwdg_cnt;
    WWDG->CR |= 1 << 7; //start
	if(EWI_enable){ //err not fix
        MY_NVIC_Init(2, 3, WWDG_IRQn, 2);
        WWDG->SR = 0X00; 
        WWDG->CFR |= 1 << 9; 
    }
}

void wwdg_feed(){
    WWDG->CR = wwdg_cnt;
}


#ifdef __cplusplus
extern "C" {
#endif
	
void WWDG_IRQHandler(void)
{
	WWDG->CR = wwdg_cnt;
	WWDG->SR = 0X00; 
}

#ifdef __cplusplus
}
#endif
