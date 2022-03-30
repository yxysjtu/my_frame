/*
 * @Description: led driver
 * @Author: yu
 * @LastEditTime: 2022-02-11 21:21:36
 */
#include "led.h"


pin led0 = {GPIOB, 5};
pin led1 = {GPIOE, 5};

void led_init(){
    pinMode(led0, OUTPUT, HIGH);
    pinMode(led1, OUTPUT, HIGH);
}
