#ifndef __USART1_H
#define __USART1_H
#include "sys.h"

void Usart1_Init(u32 bound);
void Usart1_SendChar(uint8_t c);
void Usart1_SendString(char *str);
uint8_t Usart1_ReceiveChar(void);

#endif


