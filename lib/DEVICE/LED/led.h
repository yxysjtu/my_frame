/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-11 21:21:34
 */
#ifndef LED_H
#define LED_H
	
#include "basic.h"

extern pin led0, led1;
#define LED0 GPIOout(led0)
#define LED1 GPIOout(led1)

void led_init(void);

#endif
