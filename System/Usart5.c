#include "Usart5.h"
#include <stdio.h>

#if 1
#pragma import(__use_no_semihosting)             
//��׼����Ҫ��֧�ֺ���                 
struct __FILE 
{ 
	int handle; 
}; 

FILE __stdout;       
//����_sys_exit()�Ա���ʹ�ð�����ģʽ    
void _sys_exit(int x) 
{ 
	x = x; 
} 
//�ض���fputc��������printf����ض���UART5
int fputc(int ch, FILE *f)
{ 	
	while((UART5->SR&0X40)==0);//ѭ������,ֱ���������   
	UART5->DR = (u8) ch;      
	return ch;
}
#endif

void Usart5_Init(u32 bound)
{
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD, ENABLE); //ʹ��GPIOC��GPIODʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART5, ENABLE); //ʹ��UART5ʱ��
 
	//UART5�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12; //PC12 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_Init(GPIOC, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2; //PD2 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOD, &GPIO_InitStructure);

  //UART5 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(UART5, &USART_InitStructure); //��ʼ������5
	
	//UART5 NVIC ����
  NVIC_InitStructure.NVIC_IRQChannel = UART5_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 3; //��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;        //�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;           //IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���
  
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE); //�������ڽ����ж�
  USART_Cmd(UART5, ENABLE);  //ʹ�ܴ���5
}

// ����һ���ַ�
void Usart5_SendChar(uint8_t c)
{
  while(USART_GetFlagStatus(UART5, USART_FLAG_TC) == RESET);
  USART_SendData(UART5, c);
}

// �����ַ���
void Usart5_SendString(char *str)
{
  while(*str)
  {
    Usart5_SendChar(*str++);
  }
}

// ����һ���ַ�������ʽ��
uint8_t Usart5_ReceiveChar(void)
{
  while(USART_GetFlagStatus(UART5, USART_FLAG_RXNE) == RESET);
  return USART_ReceiveData(UART5);
}

// UART5�жϴ�����
void UART5_IRQHandler(void)                	
{
	uint8_t recv_data;
	
	if(USART_GetITStatus(UART5, USART_IT_RXNE) != RESET)  //�����ж�
	{
		recv_data = USART_ReceiveData(UART5); //��ȡ���յ�������
		

		USART_ClearITPendingBit(UART5, USART_IT_RXNE); //����жϱ�־λ
	}
}
