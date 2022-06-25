/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-13 18:53:09
 */
#include "stepper.h"

Stepper* stepper_member[8];
u8 stepper_cnt = 0;

void stepper_itr(){
    for(u8 i = 0; i < stepper_cnt; i++){
        stepper_member[i]->run();
    }
}

Stepper::Stepper(Stepper_pin p){
	this->p = p;
    pinMode(p.An, OUTPUT);
	pinMode(p.Ap, OUTPUT);
	pinMode(p.Bn, OUTPUT);
	pinMode(p.Bp, OUTPUT);
	
    phase = 0;
    deg = 0;
	current_deg = 0;
}

void Stepper::init(timer &tim){
    stepper_member[stepper_cnt++] = this;
	this->tim = tim.tim;
    tim.init();
    tim.set_frequency(500);
    tim.attach_ITR(stepper_itr);
    tim.enable();
}

pin Stepper::getpin(){
	switch(phase){
		case 0: return p.Ap;
		case 1: return p.Bp;
		case 2: return p.An;
		case 3: return p.Bn;
	}
	return p.Ap;
}
void Stepper::run(){
    if(deg > 0){
        deg--;
		current_deg++;
        GPIOout(getpin()) = 0;
        if(++phase >= 4) phase = 0;
        GPIOout(getpin()) = 1;
    }else if(deg < 0){
        deg++;
		current_deg--;
        GPIOout(getpin()) = 0;
        if(--phase < 0) phase = 3;
        GPIOout(getpin()) = 1;
    }else{
        GPIOout(getpin()) = 0;
    }
}
void Stepper::spin(float deg){
    this->deg = deg / 45 * 256;
}
void Stepper::set_pos(float deg){
	spin(deg - current_deg);
}
u8 Stepper::is_stop(){
    return deg == 0;
}
float Stepper::angle(){
	return (float)current_deg * 45 / 256;
}
void Stepper::set_speed(float v){
    float f = v;
    
    u32 arr = 72000000 / f;
    u32 psc = 1;
    while(arr > 65536){
        arr >>= 1;
        psc <<= 1;
    }
    tim->ARR = arr - 1;
    tim->PSC = psc - 1; 
}
