#ifndef __USART5_H
#define __USART5_H
#include "sys.h"
#include "stm32f10x.h"  // �޸�ΪF1ƽ̨ͷ�ļ�
#include <stdio.h>      // ���stdio.h֧��printf


void Usart5_Init(u32 bound);
void Usart5_SendChar(uint8_t c);
void Usart5_SendString(char *str);
uint8_t Usart5_ReceiveChar(void);

#endif
