/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-02 13:57:51
 */
#ifndef EXTI_H
#define EXTI_H
	
#include "basic.h"

typedef enum{
    RISING = 2,
    FALLING = 1,
    CHANGING = 3
} detect_mode;


//same pin num share 1 interrupt, like PA1,PB1,PC1...
void attach_ITR(pin p, detect_mode dmode, void (*f)(void), NVIC_priority priority=default_priority);

#endif
