#include "encoder.h"
#include "delay.h"

/**************************************************************************
���ܣ���ʼ��������1 (TIM1 - PE9/PE11)
��ڲ�������
����  ֵ����
˵����ʹ��TIM1��Ϊ��������PE9/PE11���ţ���ȫ��ӳ��
**************************************************************************/
void Encoder1_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // ʹ��ʱ��
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM1 | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO, ENABLE);
    
    // ������ӳ��
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM1, ENABLE);
    
    // ����GPIO����Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_11;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOE, &GPIO_InitStructure);
    
    // ���ö�ʱ����������
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInitStrue.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM1, &TIM_TimeBaseInitStrue);
    
    // ���ñ������ӿ�ģʽ��CH2���Է�ת���������������
    TIM_EncoderInterfaceConfig(TIM1, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Falling);
    
    // ���������
    TIM_SetCounter(TIM1, 0);
    
    // �߼���ʱ����Ҫ�����ʹ��
    TIM_CtrlPWMOutputs(TIM1, ENABLE);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM1, ENABLE);
}

/**************************************************************************
���ܣ���ʼ��������2 (TIM2 - PA15/PB3)
��ڲ�������
����  ֵ����
˵����ʹ��TIM2��Ϊ��������PA15/PB3���ţ�������ӳ��1
**************************************************************************/
void Encoder2_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // ʹ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
    
    // ����JTAG���ܣ��ͷ�PA15����TIM2������
    GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE);
    GPIO_PinRemapConfig(GPIO_PartialRemap1_TIM2, ENABLE);
    
    // ����PA15����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOA, &GPIO_InitStructure);
    
    // ����PB3����
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
    
    // ���ö�ʱ����������
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStrue);
    
    // ���ñ������ӿ�ģʽ
    TIM_EncoderInterfaceConfig(TIM2, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // ���������
    TIM_SetCounter(TIM2, 0);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM2, ENABLE);
}

/**************************************************************************
���ܣ���ʼ��������3 (TIM3 - PC6/PC7)
��ڲ�������
����  ֵ����
˵����ʹ��TIM3��Ϊ��������PC6/PC7���ţ���ȫ��ӳ�䣬��������ת
**************************************************************************/
void Encoder3_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // ʹ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC | RCC_APB2Periph_AFIO, ENABLE);
    
    // ������ӳ��
    GPIO_PinRemapConfig(GPIO_FullRemap_TIM3, ENABLE);
    
    // ����GPIO����Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // ���ö�ʱ����������
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM3, &TIM_TimeBaseInitStrue);
    
    // ���ñ������ӿ�ģʽ��CH2���Է�ת���������������
    TIM_EncoderInterfaceConfig(TIM3, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // ���������
    TIM_SetCounter(TIM3, 0);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM3, ENABLE);
}

/**************************************************************************
���ܣ���ʼ��������4 (TIM4 - PD12/PD13)
��ڲ�������
����  ֵ����
˵����ʹ��TIM4��Ϊ��������PD12/PD13���ţ���ӳ��
**************************************************************************/
void Encoder4_Init(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    
    // ʹ��ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOD | RCC_APB2Periph_AFIO, ENABLE);
    
    // ������ӳ��
    GPIO_PinRemapConfig(GPIO_Remap_TIM4, ENABLE);
    
    // ����GPIO����Ϊ��������
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    // ���ö�ʱ����������
    TIM_TimeBaseInitStrue.TIM_Period = 0xFFFF;
    TIM_TimeBaseInitStrue.TIM_Prescaler = 0;
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM4, &TIM_TimeBaseInitStrue);
    
    // ���ñ������ӿ�ģʽ
    TIM_EncoderInterfaceConfig(TIM4, TIM_EncoderMode_TI12, 
                              TIM_ICPolarity_Rising, TIM_ICPolarity_Rising);
    
    // ���������
    TIM_SetCounter(TIM4, 0);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM4, ENABLE);
}

/**************************************************************************
���ܣ���ʼ�����б�����
��ڲ�������
����  ֵ����
˵������ʼ��TIM1��TIM2��TIM3��TIM4Ϊ������ģʽ
**************************************************************************/
void Encoder_Init(void)
{
    Encoder1_Init(); // TIM1
    Encoder2_Init(); // TIM2
    Encoder3_Init(); // TIM3
    Encoder4_Init(); // TIM4
}

/**************************************************************************
���ܣ���ȡָ����������ǰCNTֵ
��ڲ�����������
����  ֵ����������ǰCNTֵ���з��ţ�֧����������
˵����ֱ�Ӷ�ȡ��ʱ��������ֵ
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
���ܣ�����ָ������������
��ڲ�����������
����  ֵ����
˵������ָ����ʱ������������
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
���ܣ��������б���������
��ڲ�������
����  ֵ����
˵���������б���������������
**************************************************************************/
void Reset_All_Encoders(void)
{
    TIM_SetCounter(TIM1, 0);
    TIM_SetCounter(TIM2, 0);
    TIM_SetCounter(TIM3, 0);
    TIM_SetCounter(TIM4, 0);
}
