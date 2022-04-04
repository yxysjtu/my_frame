/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 09:16:31
 */
#ifndef TPAD_H
#define TPAD_H

#include "basic.h"

void tpad_init();
void tpad_attach_ITR(void (*f)(void));

void tpad_reset();
int32_t tpad_get_val();
u8 tpad_ispressed();

#endif
