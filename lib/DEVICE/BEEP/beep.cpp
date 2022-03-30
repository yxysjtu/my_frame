/*
 * @Description: beep driver
 * @Author: yu
 * @LastEditTime: 2022-02-11 21:21:36
 */
#include "beep.h"


pin beep = {GPIOB, 8};

void beep_init(){
    pinMode(beep, OUTPUT, HIGH);
}
