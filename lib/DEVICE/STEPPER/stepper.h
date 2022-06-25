/*
 * @Description: support up to 8 steppers
 * @Author: yu
 * @LastEditTime: 2022-04-13 18:52:41
 */
#ifndef STEPPER_H
#define STEPPER_H

#include "timer.h"

typedef struct{
	pin Ap;
	pin Bp;
	pin An;
	pin Bn;
} Stepper_pin;

class Stepper{
private:
    int phase;
    int deg;
	int current_deg;
	Stepper_pin p;
    TIM_TypeDef *tim;

	pin getpin();
public:
    Stepper(Stepper_pin p);
    void init(timer &tim);

    void spin(float deg); //non-blocking
	void set_pos(float deg);
    u8 is_stop();
    void set_speed(float v); //<=500
	float angle();

    void run(); //private
};

#endif
