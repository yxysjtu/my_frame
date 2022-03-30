/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 19:34:33
 */
#ifndef WDG_H
#define WDG_H

#include "basic.h"


void iwdg_init(float tout_ms); //0.1 ~ 26000 ms
void iwdg_feed();

//but there's still interrupt issues
void wwdg_init(float tmin_ms, float tmax_ms, u8 EWI_enable=0); //0.113ms ~ 58.25ms
void wwdg_feed();

#endif
