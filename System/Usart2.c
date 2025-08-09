#include "Usart2.h"
#include "openmv_control.h"

// OpenMV数据包状态机 - 支持变长数据包
static struct {
    uint8_t state;          // 0=等待包头, 1=读取数据
    uint8_t buffer[8];      // 增大缓冲区以支持更长的数据包
    uint8_t index;
    uint8_t expected_length; // 期望的数据包长度
} packet_parser = {0};

// 获取数据包长度 - 根据第一个字节确定
static uint8_t get_packet_length(uint8_t first_byte)
{
    if(first_byte == 0xAA) {
        return 5;  // 主控制数据包: [0xAA, 模式, PID, 标志, 颜色]
    } else if(first_byte == 0x22) {  // CMD_COLOR_RESULT
        return 3;  // 颜色结果数据包: [0x22, 索引, 颜色]
    } else if(first_byte == 0x20 || first_byte == 0x21) {  // CMD_COLOR_RECORDED, CMD_COLOR_DETECT
        return 2;  // 颜色命令数据包: [命令, 索引]
    }
    return 1;  // 默认单字节
}

// 处理OpenMV数据包的状态机 - 变长版本
static void process_openmv_byte(uint8_t data)
{
    switch(packet_parser.state) {
        case 0:  // 等待包头字节
            packet_parser.buffer[0] = data;
            packet_parser.expected_length = get_packet_length(data);
            packet_parser.index = 1;
            
            if(packet_parser.expected_length > 1) {
                packet_parser.state = 1;  // 继续接收数据
            } else {
                // 单字节数据包，直接处理
                OpenMV_Process_Data(packet_parser.buffer, 1);
                packet_parser.state = 0;
                packet_parser.index = 0;
            }
            break;
            
        case 1:  // 接收数据字节
            packet_parser.buffer[packet_parser.index++] = data;
            
            if(packet_parser.index >= packet_parser.expected_length) {
                // 数据包接收完整，处理数据包
                OpenMV_Process_Data(packet_parser.buffer, packet_parser.expected_length);
                
                // 重置状态机
                packet_parser.state = 0;
                packet_parser.index = 0;
                packet_parser.expected_length = 0;
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

