#ifndef __USART2_H
#define __USART2_H
#include "sys.h"

// º¯ÊıÉùÃ÷
void Usart2_Init(u32 bound);
void Usart2_SendChar(uint8_t c);
void Usart2_SendString(char *str);

#endif
