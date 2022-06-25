#include "basic.h"
#include "usart.h"
#include "timer.h"
#include "led.h"
#include "beep.h"
#include "key.h"
//#include "exti.h"
//#include "wdg.h"
//#include "tpad.h"
#include "pwm.h"
#include "pulse.h"
#include "spwm.h"
#include "tone.h"
#include "servo.h"
#include "stepper.h"
#include "adc.h"

u8 buf[64];
u8 buf_len;

u32 t = 0;
/*float sin_buf[4000];

float sin(float x){
	float x2 = x * x, r = x, result = 0;
	for(float i = 1; i < 20; i += 2){
		result += r;
		r = -r * x2 / (i + 2) / (i + 1);
	}
	return result;
}
void set_buf(){
	for(int i = 0; i < 4000; i++){
		sin_buf[i] = sin(3.14*(float)i/4000)/2+0.5;
		uart1.printf("%.2f\r\n", sin_buf[i]);
	}
}*/
void led0_flip(){
	LED0 = ~LED0;
}
void led1_flip(){
	LED1 = ~LED1;
	/*u32 t1 = millis();
	uart1.printf("sys_t:%d; t:%d\r\n", t1, t1 - t);
	t = t1;*/
}

//Servo servo1;
/*Stepper_pin p = {{GPIOB,5},{GPIOA,1},{GPIOA,2},{GPIOA,3}};
Stepper stepper1(p);*/

int main(void){
	sys_init();
	uart1.enable(0, RECV_BY_LINE_ENDING);
	led_init();
	key_init();
	adc1.init();
	pin p1 = {GPIOA, 1};
	adc1.config_channel(1, p1, PERIOD_252);
	
	//stepper1.init(tim2);
	//stepper1.spin(1000);
	
	//servo1.init(tim3, TIM_CH4, 1);
	/*tone_init(tim4, 1 << 2);
	tone_play(8);*/
	/*set_buf();
	spwm_init(sin_buf, 4000, tim3, tim2, (1 << 1) + (1 << 3), 4000, 1);*/
	/*tim2.init();
	tim2.set_frequency(2);
	tim2.attach_ITR(led1_flip);
	tim2.enable();*/
	
	/*tim3.init();
	tim3.set_frequency(5000);
	tim3.set_channel_mode(TIM_CH2, pwm, 1);
	tim3.set_pulsewidth(TIM_CH2, 0.5);
	tim3.enable_channel(TIM_CH2);
	tim3.enable();*/
	/*pwm_init(tim2, 15, 500);
	pwm1out(0.5);pwm2out(0.5);pwm3out(0.5); pwm4out(0.5);*/
	/*pwm_init(tim3, (1 << 1) + (1 << 3), 500, 1);
	pwm1out(0.5); pwm4out(0.5);*/
	/*pulsein_init(tim5, 1 << 0);
	pulseout_init(tim3, 1 << 1, 1, 1);
	delay(2000);
	LED1 = 0;*/
	/*tim5.init();
	tim5.set_frequency(65535, 71);
	tim5.set_channel_mode(TIM_CH1, input_capture);
	tim5.attach_ITR(edge_detected, capture_compare1);
	tim5.attach_ITR(cnt_overflow, update);
	tim5.enable_channel(TIM_CH1);
	tim5.enable();*/

	/*tpad_init();
	tpad_attach_ITR(led0_flip);*/

	//iwdg_init(500);
	//delay(100);
	/*LED0 = 0;
	delay(100);
	LED0 = 1;
	wwdg_init(20,40,1);*/
	
	//attach_ITR(key[0], CHANGING, led0_flip);
	//attach_ITR(key[3], RISING, led1_flip);
	//int st = 10;
	while(1){
		//LED1 = ~LED1;
		//delay(500);
		uart1.printf("%.2f\r\n", adc1.read_aver(1, 10));
		/*GPIOout(p.Ap) = 1;
		GPIOout(p.Bp) = 0;
		GPIOout(p.An) = 0;
		GPIOout(p.Bn) = 0;
		delay(st);
		GPIOout(p.Ap) = 0;
		GPIOout(p.Bp) = 1;
		GPIOout(p.An) = 0;
		GPIOout(p.Bn) = 0;
		delay(st);
		GPIOout(p.Ap) = 0;
		GPIOout(p.Bp) = 0;
		GPIOout(p.An) = 1;
		GPIOout(p.Bn) = 0;
		delay(st);
		GPIOout(p.Ap) = 0;
		GPIOout(p.Bp) = 0;
		GPIOout(p.An) = 0;
		GPIOout(p.Bn) = 1;
		delay(st);*/
		/*if(uart1.readline(buf, &buf_len)){
			int deg = to_int(buf, buf_len);
			if(deg >= 0 && deg <= 180) servo1.out(deg);
			//uart1.printf("recv:%d\r\n", to_int(buf, buf_len));
		}*/
		/*if(uart1.readline(buf, &buf_len)){
			int sep;
			for(sep = 0; sep < buf_len && buf[sep] != ' '; sep++);
			if(sep < buf_len - 1){
				int deg = to_int(buf, sep);
				int v = to_int(buf + sep + 1, buf_len - sep - 1); 
				stepper1.set_speed(v);
				stepper1.spin(deg);
				uart1.printf("spin:%d, v:%d, ang:%.2f\r\n", deg, v, stepper1.angle());
			}
		}*/
		/*uart1.printf("t:%dms\r\n", millis());
		for(u32 i = 0; i < 1000000; i++);*/
		//uart1.printf("t:%dms, %dus\r\n", millis(), micros());
		/*LED0 = ~LED0;
		delay_us(50000);*/
		//delay(500);
		/*for(u8 i = 0; i < 180; i++){
			servo1.out(i);
			delay(5);
		}
		for(u8 i = 180; i > 0; i--){
			servo1.out(i);
			delay(5);
		}*/
		/*pulseout(2, 0.5, 3);
		delay(5000);*/
		//if((cnt = pulsein(1)) != 0) uart1.printf("pulse:%dms\r\n", cnt / 1000);
		/*tpad_reset();
		LED1 = ~LED1;
		delay(500);*/
		//tpad_get_val();
		
		/*for(float i = 1; i >= 0; i -= 0.01){
			pwm2out(i);
			//tim3.set_pulsewidth(TIM_CH2, i);
			delay(10);
		}
		for(float i = 0; i <= 1; i += 0.01){
			//tim3.set_pulsewidth(TIM_CH2, i);
			pwm2out(i);
			delay(10);
		}*/
		//delay(19);
		//wwdg_feed();

		//if(KEY0 == 0) iwdg_feed();
		//uart1.printf("helloworld!!!\r\n");
		//delay(1000);
		/*if(KEY1 == 0) LED0 = 1;
		else LED0 = 0;*/
		/*LED1 = ~LED1;
		uart1.printf("helloworld!!!\r\n");
		delay(500);*/
		/*buf_len = 10;
		if(uart1.read(buf, buf_len)){
			uart1.write(buf, buf_len);
			uart1.printf("\r\n10\r\n");
		}*/
		/*if(uart1.readline(buf, &buf_len)){
			while(!uart1.write_available());
			uart1.write(buf, buf_len);
			while(!uart1.write_available());
			uart1.printf("helloworld!!!\r\n");
		}*/
		//uart1.printf("remain:%d\r\n",uart1.rdma.DMA_CHx->CNDTR);
		/*uart1.printf("helloworld\r\n");
		delay(200);*/
		/*LED0 = ~LED0;
		delay(500);*/
	}
	
	//return 0;
}

