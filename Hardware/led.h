#ifndef __LED_H
#define __LED_H
#include "sys.h"




//LED�˿ڶ���
#define LED0 PAout(8)	// DS0
#define LED1 PBout(4)	// DS1	 

void LED_Init(void);//��ʼ��	
void LED_ON(unsigned char LED);//����LED
void LED_OFF(unsigned char LED);//Ϩ��LED
#endif
