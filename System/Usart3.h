#ifndef __USART3_H
#define __USART3_H

#include "sys.h"


void Usart3_Init(u32 bound);
void USART3_SendChar(uint8_t c);
void USART3_SendString(char *str);
uint8_t USART3_ReceiveChar(void);

#endif 
