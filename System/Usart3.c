#include "Usart3.h"

void Usart3_Init(u32 bound)
{
   //GPIO�˿�����
  GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE); //ʹ��GPIOBʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);//ʹ��USART3ʱ��
 
	//USART3�˿�����
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10; //PB10 TX
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP; //�����������
	GPIO_Init(GPIOB, &GPIO_InitStructure);
   
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11; //PB11 RX
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOB, &GPIO_InitStructure);

  //USART3 ��ʼ������
	USART_InitStructure.USART_BaudRate = bound;//����������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;//�ֳ�Ϊ8λ���ݸ�ʽ
	USART_InitStructure.USART_StopBits = USART_StopBits_1;//һ��ֹͣλ
	USART_InitStructure.USART_Parity = USART_Parity_No;//����żУ��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//��Ӳ������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;//�շ�ģʽ
  USART_Init(USART3, &USART_InitStructure); //��ʼ������3
	
  USART_Cmd(USART3, ENABLE);  //ʹ�ܴ���3
}

// ����һ���ַ�
void USART3_SendChar(uint8_t c)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET);
    USART_SendData(USART3, c);
}

// �����ַ���
void USART3_SendString(char *str)
{
    while(*str)
    {
        USART3_SendChar(*str++);
    }
}

// ����һ���ַ�������ʽ��
uint8_t USART3_ReceiveChar(void)
{
    while(USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET);
    return USART_ReceiveData(USART3);
} 
