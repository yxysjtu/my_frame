/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 15:05:23
 */
#ifndef KEY_H
#define KEY_H
	
#include "basic.h"

extern pin key[4];
#define KEY0 GPIOin(key[0])
#define KEY1 GPIOin(key[1])
#define KEY2 GPIOin(key[2])
#define WK_UP GPIOin(key[3])

void key_init(void);

#endif
