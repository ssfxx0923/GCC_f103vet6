#ifndef __USART2_H
#define __USART2_H
#include "sys.h"

void Usart2_Init(u32 bound);
void Usart2_SendChar(uint8_t c);
void Usart2_SendString(char *str);
uint8_t Usart2_ReceiveChar(void);

#endif
