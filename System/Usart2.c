#include "Usart2.h"
#include "openmv_control.h"

// OpenMV数据包状态机 - 5字节协议：[0xAA, 模式位, PID位, 交叉线标志位, 颜色位]
static struct {
    uint8_t state;          // 0=等待0xAA, 1=读模式位, 2=读PID位, 3=读交叉线标志位, 4=读颜色位
    uint8_t buffer[5];
    uint8_t index;
} packet_parser = {0};

// 处理OpenMV数据包的状态机 - 5字节版本
static void process_openmv_byte(uint8_t data)
{
    switch(packet_parser.state) {
        case 0:  // 等待同步字节 0xAA
            if(data == 0xAA) {
                packet_parser.buffer[0] = data;
                packet_parser.state = 1;
                packet_parser.index = 1;
            }
            break;
            
        case 1:  // 读取模式位
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 2;
            break;
            
        case 2:  // 读取PID位
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 3;
            break;
            
        case 3:  // 读取交叉线标志位
            packet_parser.buffer[packet_parser.index++] = data;
            packet_parser.state = 4;
            break;
            
        case 4:  // 读取颜色位，完成数据包
            packet_parser.buffer[packet_parser.index] = data;
            
            // 处理完整的数据包
            OpenMV_Process_Data(packet_parser.buffer, 5);
            
            // 重置状态机
            packet_parser.state = 0;
            packet_parser.index = 0;
            
            // 检查当前字节是否是新包的开始
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
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 配置TX引脚 (PA2)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置RX引脚 (PA3)
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_Init(GPIOA, &GPIO_InitStructure);

    // 配置USART2参数
    USART_InitStructure.USART_BaudRate = bound;
    USART_InitStructure.USART_WordLength = USART_WordLength_8b;
    USART_InitStructure.USART_StopBits = USART_StopBits_1;
    USART_InitStructure.USART_Parity = USART_Parity_No;
    USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
    USART_Init(USART2, &USART_InitStructure);
    
    // 配置中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能接收中断和USART2
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    USART_Cmd(USART2, ENABLE);
}

// USART2中断服务程序
void USART2_IRQHandler(void)
{
    if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        uint8_t data = USART_ReceiveData(USART2);
        
        // 处理OpenMV数据包
        process_openmv_byte(data);
        
        USART_ClearITPendingBit(USART2, USART_IT_RXNE);
    }
}

// 发送单个字符
void Usart2_SendChar(uint8_t c)
{
    while(USART_GetFlagStatus(USART2, USART_FLAG_TC) == RESET);
    USART_SendData(USART2, c);
}

// 发送字符串
void Usart2_SendString(char *str)
{
    while(*str) {
        Usart2_SendChar(*str++);
    }
}

