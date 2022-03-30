/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-02-27 15:08:48
 */
#include "key.h"

pin key[4] = {{GPIOE, 4},
              {GPIOE, 3},
              {GPIOE, 2},
              {GPIOA, 0}};

void key_init(){
    pinMode(key[0], INPUT, HIGH);
    pinMode(key[1], INPUT, HIGH);
    pinMode(key[2], INPUT, HIGH);
    pinMode(key[3], INPUT, LOW);
}
