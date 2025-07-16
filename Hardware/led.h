#ifndef __LED_H
#define __LED_H
#include "sys.h"




//LED端口定义
#define LED0 PAout(8)	// DS0
#define LED1 PBout(4)	// DS1	 

void LED_Init(void);//初始化	
void LED_ON(unsigned char LED);//点亮LED
void LED_OFF(unsigned char LED);//熄灭LED
#endif
