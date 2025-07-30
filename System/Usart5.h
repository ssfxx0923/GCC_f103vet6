#ifndef __USART5_H
#define __USART5_H
#include "sys.h"
#include "stm32f10x.h"  // 修改为F1平台头文件
#include <stdio.h>      // 添加stdio.h支持printf


void Usart5_Init(u32 bound);
void Usart5_SendChar(uint8_t c);
void Usart5_SendString(char *str);
uint8_t Usart5_ReceiveChar(void);

#endif
