#include "Usart2.h"
#include "openmv_control.h"

// OpenMV���ݰ�״̬�� - 5�ֽ�Э�飺[0xAA, ģʽλ, PIDλ, �����߱�־λ, ��ɫλ]
static struct {
    uint8_t state;          // 0=�ȴ�0xAA, 1=��ģʽλ, 2=��PIDλ, 3=�������߱�־λ, 4=����ɫλ
    uint8_t buffer[5];
    uint8_t index;
} packet_parser = {0};

// ����OpenMV���ݰ���״̬�� - 5�ֽڰ汾
static void process_openmv_byte(uint8_t data)
{
    switch(packet_parser.state) {
        case 0:  // �ȴ�ͬ���ֽ� 0xAA
            if(data == 0xAA) {
                packet_parser.buffer[0] = data;
                packet_parser.state = 1;
                packet_parser.index = 1;
            }
            break;
            
        case 1:  // ��ȡģʽλ
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 2;
            break;
            
        case 2:  // ��ȡPIDλ
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 3;
            break;
            
        case 3:  // ��ȡ�����߱�־λ
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 4;
            break;
            
        case 4:  // ��ȡ��ɫλ��������ݰ�
            packet_parser.buffer[packet_parser.index] = data;
            
            // �������������ݰ�
            OpenMV_Process_Data(packet_parser.buffer, 5);
            
            // ����״̬��
            packet_parser.state = 0;
            packet_parser.index = 0;
            
            // ��鵱ǰ�ֽ��Ƿ����°��Ŀ�ʼ
            if(data == 0xAA) {
                packet_parser.buffer[0] = data;
                packet_parser.state = 1;
                packet_parser.index = 1;
            }
            break;
    }
}

void Usart2_Init(u32 bound)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // ʹ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // ����TX���� (PA2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ����RX���� (PA3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // ����USART2����
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    // �����ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ʹ�ܽ����жϺ�USART2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

// USART2�жϷ������
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART2);
        
        // ����OpenMV���ݰ�
        process_openmv_byte(data);
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// ���͵����ַ�
void Usart2_SendChar(uint8_t c)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, c);
}

// �����ַ���
void Usart2_SendString(char *str)
{
    while(*str) {
        Usart2_SendChar(*str++);
    }
}

