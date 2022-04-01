#include "basic.h"
#include "ITR.h"
#include "usart.h"
#include "led.h"
#include "beep.h"
#include "key.h"
#include "exti.h"
#include "wdg.h"

u8 buf[64];
u8 buf_len;

void led0_flip(){
	LED0 = ~LED0;
}
void led1_flip(){
	LED1 = ~LED1;
}

int main(void){
	sys_init();
	uart1.begin();
	led_init();
	key_init();
	//iwdg_init(500);
	//delay(100);
	/*LED0 = 0;
	delay(100);
	LED0 = 1;
	wwdg_init(20,40,1);*/
	
	//attach_ITR(key[0], CHANGING, led0_flip);
	//attach_ITR(key[3], RISING, led1_flip);
	
	while(1){
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
			//LED0 = ~LED0;
		}*/
		//uart1.printf("remain:%d\r\n",uart1.rdma.DMA_CHx->CNDTR);
		/*uart1.printf("helloworld\r\n");
		delay(200);*/
	}
	
	//return 0;
}

