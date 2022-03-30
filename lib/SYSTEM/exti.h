/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 14:49:08
 */
#ifndef EXTI_H
#define EXTI_H
	
#include "basic.h"

typedef enum{
    RISING = 2,
    FALLING = 1,
    CHANGING = 3
} detect_mode;

extern NVIC_priority default_priority;

void attach_ITR(pin p, detect_mode dmode, void (*f)(void), NVIC_priority priority=default_priority);

#endif
