#ifndef __USART5_H
#define __USART5_H
#include "sys.h"
#include "stm32f10x.h"  // 修改为F1平台头文件
#include <stdio.h>      // 添加stdio.h支持printf

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//STM32F103VE开发板
//UART5驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2024/5/2
//版本：V1.0
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2009-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

void Usart5_Init(u32 bound);
void Usart5_SendChar(uint8_t c);
void Usart5_SendString(char *str);
uint8_t Usart5_ReceiveChar(void);

#endif
