#ifndef __USART4_H
#define __USART4_H
#include "sys.h"


void Usart4_Init(u32 bound);
void Usart4_SendChar(uint8_t c);
void Usart4_SendString(char *str);
uint8_t Usart4_ReceiveChar(void);

#endif
