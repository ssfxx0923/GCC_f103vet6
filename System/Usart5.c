#include "Usart5.h"
#include <stdio.h>

#if 1
#pragma import(__use_no_semihosting)             
//标准库需要的支持函数                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//定义_sys_exit()以避免使用半主机模式    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//重定义fputc函数，将printf输出重定向到UART5
int fputc(int ch, FILE *f)
{ 	
	while((UART5->SR&0X40)==0);//循环发送,直到发送完毕   
	UART5->DR = (u8) ch;      
	return ch;
}
#endif

void Usart5_Init(u32 bound)
{
   //GPIO端口设置
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); //使能GPIOC和GPIOD时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //使能UART5时钟
 
	//UART5端口配置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //复用推挽输出
	GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PD2 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  //UART5 初始化设置
	USART_InitStructure.USART_BaudRate = bound;//波特率设置
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//字长为8位数据格式
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//一个停止位
	USART_InitStructure.USART_Parity = USART_Parity_No;//无奇偶校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//无硬件数据流控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//收发模式
  USART_Init(UART5, &USART_InitStructure); //初始化串口5
	
  USART_Cmd(UART5, ENABLE);  //使能串口5
}

// 发送一个字符
void Usart5_SendChar(uint8_t c)
{
  while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
  USART_SendData(UART5, c);
}

// 发送字符串
void Usart5_SendString(char *str)
{
  while(*str)
  {
    Usart5_SendChar(*str++);
  }
}

// 接收一个字符（阻塞式）
uint8_t Usart5_ReceiveChar(void)
{
  while(USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET);
  return USART_ReceiveData(UART5);
}
