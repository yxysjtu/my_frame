/*
 * @Description: 
 * @Author: yu
 * @LastEditTime: 2022-04-04 09:36:46
 */
#include "tpad.h"
#include "timer.h"

int32_t tpad_default_val = 0, tpad_val;
u8 tpad_detect_flag = 0, tpad_state = 2;
void (*tpad_func)(void);

void tpad_reset(){
	pin p = {GPIOA, 1};
	pinMode(p, OUTPUT, LOW); //discharge
	delay(20);
	tpad_detect_flag = 0;
	pinMode(p, FLOAT_INPUT); //charge
	tpad_val = TIM5->CNT;
}
void tpad_detect_press(){
	tpad_val = TIM5->CCR2 - tpad_val;
	if(tpad_val <= 0) tpad_val += 65536;
	tpad_detect_flag = 1;

    if(tpad_state != 2){
        int err = 50;
        if(tpad_val - tpad_default_val > err){
            if(!tpad_state){
                if(tpad_func != 0) tpad_func();
                tpad_state = 1;
            }    
        }else tpad_state = 0;
    }
}
int32_t tpad_get_val(){
	tpad_reset();
	while(!tpad_detect_flag);
	return tpad_val;
}
void tpad_init(){
	u16 buf[10], temp;
	tpad_default_val = 0; 
    tpad_func = 0;
    tpad_state = 2;

    tim5.init();
	tim5.set_frequency(65535, 71);
	tim5.set_channel_mode(TIM_CH2, input_capture);
	tim5.attach_ITR(tpad_detect_press, capture_compare2);
	tim5.enable_channel(TIM_CH2);
	tim5.enable();
    
	for(u8 i = 0; i < 10; i++){
		buf[i] = tpad_get_val();
	}
	for(u8 i = 9; i > 0; i--){
		for(u8 j = 0; j < i; j++){
			if(buf[j] > buf[j + 1]){
				temp = buf[i];
				buf[i] = buf[i + 1];
				buf[i + 1] = temp;
			}
		}
	}
	for(u8 i = 2; i < 8; i++) tpad_default_val += buf[i];
	tpad_default_val /= 6;

    tpad_state = 0;
	//uart1.printf("tpad_default_val:%d\r\n", tpad_default_val);
}
u8 tpad_ispressed(){
	int err = 50;
	tpad_get_val();
	if(tpad_val - tpad_default_val > err) return 1;
	return 0;
}

void tpad_attach_ITR(void (*f)(void)){
    tpad_func = f;
}
