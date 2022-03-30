/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-11 21:21:34
 */
#ifndef BEEP_H
#define BEEP_H

#include "basic.h"

extern pin beep;
#define BEEP GPIOout(beep)

void beep_init(void);

#endif
