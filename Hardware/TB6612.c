#include "TB6612.h"
#include "encoder.h"
#include "pid_control.h"



/**************************************************************************
���ܣ�TB6612��ʼ������
��ڲ�������
����  ֵ����
**************************************************************************/
void MOTOR_Init(void)
{
	// ��ʼ��Ӳ��PWM
	Hardware_PWM_Init();
	// ��ʼ�������������
	Motor_Direction_Init();
	// ֹͣ���е��
	Move_stop();
}

/**************************************************************************
���ܣ�Ӳ��PWM��ʼ��
��ڲ�������
����  ֵ����
˵����ʹ��TIM5���PWM��PA0(���2)��PA1(���1)
     ʹ��TIM8���PWM��PC8(���4)��PC9(���3)
**************************************************************************/
void Hardware_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	// ����GPIOΪ�����������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ����PA0 (���2) �� PA1 (���1) ΪTIM5��PWM���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// ����PC8 (���4) �� PC9 (���3) ΪTIM8��PWM���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// ����TIM5ʱ����Ԫ
	TIM_TimeBaseStructure.TIM_Period = 99;                      // �Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // Ԥ��Ƶֵ������Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	// ����TIM8ʱ����Ԫ
	TIM_TimeBaseStructure.TIM_Period = 99;                      // �Զ���װ��ֵ
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // Ԥ��Ƶֵ������Ƶ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // ʱ�ӷָ�
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ���ϼ���ģʽ
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	// ����PWM����ȽϽṹ��
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           // PWMģʽ1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // ���ʹ��
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // ������Ը�
	TIM_OCInitStructure.TIM_Pulse = 0;                          // ��ʼռ�ձ�Ϊ0
	
	// ����TIM5��PWM���ͨ��
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  // PA0 - TIM5 CH1 (���2)
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  // PA1 - TIM5 CH2 (���1)
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	// ����TIM8��PWM���ͨ��
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  // PC8 - TIM8 CH3 (���4)
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  // PC9 - TIM8 CH4 (���3)
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	// ������ʱ��
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
	// TIM8��Ҫ����������ʹ��
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

/**************************************************************************
� ��ܣ��������������ų�ʼ��
��ڲ�������
����  ֵ����
**************************************************************************/
void Motor_Direction_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// ʹ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);
	
	// ���÷����������Ϊ�������
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ���1����������ţ�PB0, PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// ���2����������ţ�PC4, PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// ���3����������ţ�PB5, PE14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// ���4����������ţ�PE4, PE5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**************************************************************************
���ܣ�����ָ������ķ���
��ڲ����������ţ�����1��ת��-1��ת��0ֹͣ��
����  ֵ����
**************************************************************************/
void Motor_SetDirection(MotorNum_t motor, int direction)
{
	switch(motor) {
		case Motor1:
			if (direction > 0) {
				// ���1��ת
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			} else if (direction < 0) {
				// ���1��ת
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_SetBits(GPIOB, GPIO_Pin_1);
			} else {
				// ���1ֹͣ
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			}
			break;
			
		case Motor2:
			if (direction > 0) {
				// ���2��ת
				GPIO_SetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			} else if (direction < 0) {
				// ���2��ת
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_SetBits(GPIOC, GPIO_Pin_5);
			} else {
				// ���2ֹͣ
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			}
			break;
			
		case Motor3:
			if (direction > 0) {
				// ���3��ת
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			} else if (direction < 0) {
				// ���3��ת
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_SetBits(GPIOE, GPIO_Pin_14);
			} else {
				// ���3ֹͣ
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			}
			break;
			
		case Motor4:
			if (direction < 0) {
				// ���4��ת
				GPIO_SetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			} else if (direction > 0) {
				// ���4��ת
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_SetBits(GPIOE, GPIO_Pin_5);
			} else {
				// ���4ֹͣ
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			}
			break;
	}
}

/**************************************************************************
���ܣ�����ָ�������PWMֵ
��ڲ����������ţ�PWMֵ��0-100��
����  ֵ����
**************************************************************************/
void Motor_SetPWM(MotorNum_t motor, int pwm)
{
	// ����PWMֵ��Χ
	if (pwm > PWM_MAX_VALUE) {
		pwm = PWM_MAX_VALUE;
	} else if (pwm < 0) {
		pwm = 0;
	}
	
	    switch(motor) {
        case Motor1:
            TIM_SetCompare2(TIM5, pwm);  // PA1 - TIM5 CH2
            break;
        case Motor2:
            TIM_SetCompare1(TIM5, pwm);  // PA0 - TIM5 CH1
            break;
        case Motor3:
            TIM_SetCompare4(TIM8, pwm);  // PC9 - TIM8 CH4
            break;
        case Motor4:
            TIM_SetCompare3(TIM8, pwm);  // PC8 - TIM8 CH3
            break;
    }
}

/**************************************************************************
���ܣ�ֹͣ���е��
��ڲ�������
����  ֵ����
**************************************************************************/
void Move_stop(void)
{
	Motor_SetDirection(Motor1, 0);
	Motor_SetDirection(Motor2, 0);
	Motor_SetDirection(Motor3, 0);
	Motor_SetDirection(Motor4, 0);
	
	Motor_SetPWM(Motor1, 0);
	Motor_SetPWM(Motor2, 0);
	Motor_SetPWM(Motor3, 0);
	Motor_SetPWM(Motor4, 0);
}

/**************************************************************************
���ܣ����õ��PWM�ͷ���
��ڲ����������ţ�PWMֵ��-100��100��
����  ֵ����
**************************************************************************/
void Set_Motor_PWM(MotorNum_t motor, int pwm)
{
	if(motor >= 4) return; // �������
	
	#define Dead_Voltage 10      // ��������ֵ
    #define Amplitude 90        // PWM����ֵ
	
	if(pwm > 0) {
		// ��ת
		Motor_SetDirection(motor, 1);
		pwm = pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else if(pwm < 0) {
		// ��ת
		Motor_SetDirection(motor, -1);
		pwm = -pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else {
		// ֹͣ
		Motor_SetDirection(motor, 0);
		Motor_SetPWM(motor, 0);
	}
}
