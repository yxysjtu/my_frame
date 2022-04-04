/*
 * @Description: support only 1 timer for pulsein or pulseout
 * @Author: yu
 * @LastEditTime: 2022-04-04 12:52:07
 */
#ifndef PULSE_H
#define PULSE_H

#include "timer.h"

//enable_channel: each digit stands for 1 channel. for example: 0010 -- channel2
//polarity: each digit stands for 1 channel. for example: 0010 -- channel2
void pulsein_init(timer &tim, u8 enable_channel, u8 remap=0, u8 polarity=0);
void pulseout_init(timer &tim, u8 enable_channel, u32 freq, u8 remap=0, u8 polarity=0);

u32 pulsein(u8 channel); //1 ~ 4, non-block, if no pulse return 0, else return t in us
//caution:close the output of a pulse lead to pin level reset to 0; TODO:
void pulseout(u8 channel, float duty, u32 num); //num pulse, non-block

#endif
