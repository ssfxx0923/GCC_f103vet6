#include "Usart1.h"


void Usart1_Init(u32 bound)
{
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1|RCC_APB2Periph_GPIOA, ENABLE);	//ʹ��USART1��GPIOAʱ��
  
	//USART1�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //PA.9 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;	//�����������
	GPIO_Init(GPIOA, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;//PA.10 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//��������
	GPIO_Init(GPIOA, &GPIO_InitStructure);

   //USART1 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;	//�շ�ģʽ
  USART_Init(USART1, &USART_InitStructure); //��ʼ������1
	
  USART_Cmd(USART1, ENABLE);  //ʹ�ܴ���1
}

// ����һ���ַ�
void Usart1_SendChar(uint8_t c)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_TC) == RESET);
  USART_SendData(USART1, c);
}

// �����ַ���
void Usart1_SendString(char *str)
{
  while(*str)
  {
    Usart1_SendChar(*str++);
  }
}

// ����һ���ַ�������ʽ��
uint8_t Usart1_ReceiveChar(void)
{
  while(USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == RESET);
  return USART_ReceiveData(USART1);
}



