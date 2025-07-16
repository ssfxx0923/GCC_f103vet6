#include "encoder.h"
#include "delay.h"

/**************************************************************************
功能：初始化编码器1 (TIM1 - PE9/PE11)
入口参数：无
返回  值：无
说明：使用TIM1作为编码器，PE9/PE11引脚，完全重映射
**************************************************************************/
void Encoder1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // 使能时钟
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置重映射
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
    
    // 配置GPIO引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // 配置定时器基本参数
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStrue.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStrue);
    
    // 设置编码器接口模式（CH2极性反转，计数方向调换）
    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
    
    // 清零计数器
    TIM_SetCounter(TIM1, 0);
    
    // 高级定时器需要主输出使能
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    // 使能定时器
    TIM_Cmd(TIM1, ENABLE);
}

/**************************************************************************
功能：初始化编码器2 (TIM2 - PA15/PB3)
入口参数：无
返回  值：无
说明：使用TIM2作为编码器，PA15/PB3引脚，部分重映射1
**************************************************************************/
void Encoder2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    // 禁用JTAG功能，释放PA15用于TIM2编码器
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    
    // 配置PA15引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // 配置PB3引脚
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // 配置定时器基本参数
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStrue);
    
    // 设置编码器接口模式
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 清零计数器
    TIM_SetCounter(TIM2, 0);
    
    // 使能定时器
    TIM_Cmd(TIM2, ENABLE);
}

/**************************************************************************
功能：初始化编码器3 (TIM3 - PC6/PC7)
入口参数：无
返回  值：无
说明：使用TIM3作为编码器，PC6/PC7引脚，完全重映射，计数方向反转
**************************************************************************/
void Encoder3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置重映射
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
    
    // 配置GPIO引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 配置定时器基本参数
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue);
    
    // 设置编码器接口模式（CH2极性反转，计数方向调换）
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 清零计数器
    TIM_SetCounter(TIM3, 0);
    
    // 使能定时器
    TIM_Cmd(TIM3, ENABLE);
}

/**************************************************************************
功能：初始化编码器4 (TIM4 - PD12/PD13)
入口参数：无
返回  值：无
说明：使用TIM4作为编码器，PD12/PD13引脚，重映射
**************************************************************************/
void Encoder4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // 使能时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    // 配置重映射
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
    
    // 配置GPIO引脚为浮空输入
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // 配置定时器基本参数
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStrue);
    
    // 设置编码器接口模式
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // 清零计数器
    TIM_SetCounter(TIM4, 0);
    
    // 使能定时器
    TIM_Cmd(TIM4, ENABLE);
}

/**************************************************************************
功能：初始化所有编码器
入口参数：无
返回  值：无
说明：初始化TIM1、TIM2、TIM3、TIM4为编码器模式
**************************************************************************/
void Encoder_Init(void)
{
    Encoder1_Init(); // TIM1
    Encoder2_Init(); // TIM2
    Encoder3_Init(); // TIM3
    Encoder4_Init(); // TIM4
}

/**************************************************************************
功能：获取指定编码器当前CNT值
入口参数：电机编号
返回  值：编码器当前CNT值（有符号，支持正负方向）
说明：直接读取定时器计数器值
**************************************************************************/
int16_t Get_Encoder_Count(MotorNum_t motor)
{
    switch (motor) {
        case Motor1:
            return (int16_t)TIM_GetCounter(TIM1);
        case Motor2:
            return (int16_t)TIM_GetCounter(TIM2);
        case Motor3:
            return (int16_t)TIM_GetCounter(TIM3);
        case Motor4:
            return (int16_t)TIM_GetCounter(TIM4);
        default:
            return 0;
    }
}

/**************************************************************************
功能：重置指定编码器计数
入口参数：电机编号
返回  值：无
说明：将指定定时器计数器清零
**************************************************************************/
void Reset_Encoder(MotorNum_t motor)
{
    switch (motor) {
        case Motor1:
            TIM_SetCounter(TIM1, 0);
            break;
        case Motor2:
            TIM_SetCounter(TIM2, 0);
            break;
        case Motor3:
            TIM_SetCounter(TIM3, 0);
            break;
        case Motor4:
            TIM_SetCounter(TIM4, 0);
            break;
        default:
            break;
    }
}

/**************************************************************************
功能：重置所有编码器计数
入口参数：无
返回  值：无
说明：将所有编码器计数器清零
**************************************************************************/
void Reset_All_Encoders(void)
{
    TIM_SetCounter(TIM1, 0);
    TIM_SetCounter(TIM2, 0);
    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM4, 0);
}
