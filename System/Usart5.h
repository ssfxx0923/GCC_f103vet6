#ifndef __USART5_H
#define __USART5_H
#include "sys.h"
#include "stm32f10x.h"  // �޸�ΪF1ƽ̨ͷ�ļ�
#include <stdio.h>      // ���stdio.h֧��printf

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//STM32F103VE������
//UART5��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2024/5/2
//�汾��V1.0
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2009-2024
//All rights reserved
//////////////////////////////////////////////////////////////////////////////////

void Usart5_Init(u32 bound);
void Usart5_SendChar(uint8_t c);
void Usart5_SendString(char *str);
uint8_t Usart5_ReceiveChar(void);

#endif
