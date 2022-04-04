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

u8 buf[64];
u8 buf_len;

u32 t = 0;
u32 cnt = 0;
u8 polarity = 0;

void led0_flip(){
	LED0 = ~LED0;
}
void led1_flip(){
	LED1 = ~LED1;
	/*u32 t1 = millis();
	uart1.printf("sys_t:%d; t:%d\r\n", t1, t1 - t);
	t = t1;*/
}

/*void edge_detected(){
	if(!polarity){
		polarity = 1;
		tim5.set_polarity(TIM_CH1, polarity);
		cnt = tim5.get_ccr(TIM_CH1);
		uart1.printf("press\r\n", cnt);
	}else{
		polarity = 0;
		tim5.set_polarity(TIM_CH1, polarity);
		cnt += tim5.get_ccr(TIM_CH1);
		uart1.printf("pulse:%dms\r\n", cnt / 1000);
	}
}
void cnt_overflow(){
	cnt += 65536;
}*/




int main(void){
	sys_init();
	uart1.enable();
	led_init();
	key_init();
	
	/*tim2.init();
	tim2.set_frequency(1);
	tim2.attach_ITR(led1_flip);
	tim2.enable();*/
	
	/*tim3.init();
	tim3.set_frequency(5000);
	tim3.set_channel_mode(TIM_CH2, pwm, 1);
	tim3.set_pulsewidth(TIM_CH2, 0.5);
	tim3.enable_channel(TIM_CH2);
	tim3.enable();*/
	//pwm_init(tim3, 1 << 1, 500, 1);
	pulsein_init(tim5, 1 << 0);
	pulseout_init(tim3, 1 << 1, 1, 1);
	delay(2000);
	LED1 = 0;
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
	
	while(1){
		pulseout(2, 0.5, 3);
		delay(5000);
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
		/*LED0 = ~LED0;
		delay(500);*/
		/*delay(10);
		if(dir)led0pwmval++;
		else led0pwmval--;
		if(led0pwmval>300)dir=0;
		if(led0pwmval==0)dir=1;
		LED0_PWM_VAL=led0pwmval; */
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

