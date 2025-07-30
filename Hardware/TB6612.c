#include "TB6612.h"
#include "encoder.h"
#include "pid_control.h"



/**************************************************************************
¹¦ÄÜ£ºTB6612³õÊ¼»¯º¯Êý
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void MOTOR_Init(void)
{
	// ³õÊ¼»¯Ó²¼þPWM
	Hardware_PWM_Init();
	// ³õÊ¼»¯·½Ïò¿ØÖÆÒý½Å
	Motor_Direction_Init();
	// Í£Ö¹ËùÓÐµç»ú
	Move_stop();
}

/**************************************************************************
¹¦ÄÜ£ºÓ²¼þPWM³õÊ¼»¯
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
ËµÃ÷£ºÊ¹ÓÃTIM5Êä³öPWMµ½PA0(µç»ú2)ºÍPA1(µç»ú1)
     Ê¹ÓÃTIM8Êä³öPWMµ½PC8(µç»ú4)ºÍPC9(µç»ú3)
**************************************************************************/
void Hardware_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// Ê¹ÄÜÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	// ÅäÖÃGPIOÎª¸´ÓÃÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// ÅäÖÃPA0 (µç»ú2) ºÍ PA1 (µç»ú1) ÎªTIM5µÄPWMÊä³ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// ÅäÖÃPC8 (µç»ú4) ºÍ PC9 (µç»ú3) ÎªTIM8µÄPWMÊä³ö
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// ÅäÖÃTIM5Ê±»ùµ¥Ôª
	TIM_TimeBaseStructure.TIM_Period = 99;                      // ×Ô¶¯ÖØ×°ÔØÖµ
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // Ô¤·ÖÆµÖµ£¬²»·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // Ê±ÖÓ·Ö¸î
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	// ÅäÖÃTIM8Ê±»ùµ¥Ôª
	TIM_TimeBaseStructure.TIM_Period = 99;                      // ×Ô¶¯ÖØ×°ÔØÖµ
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // Ô¤·ÖÆµÖµ£¬²»·ÖÆµ
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // Ê±ÖÓ·Ö¸î
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // ÏòÉÏ¼ÆÊýÄ£Ê½
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	// ÅäÖÃPWMÊä³ö±È½Ï½á¹¹Ìå
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           // PWMÄ£Ê½1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // Êä³öÊ¹ÄÜ
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // Êä³ö¼«ÐÔ¸ß
	TIM_OCInitStructure.TIM_Pulse = 0;                          // ³õÊ¼Õ¼¿Õ±ÈÎª0
	
	// ÅäÖÃTIM5µÄPWMÊä³öÍ¨µÀ
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  // PA0 - TIM5 CH1 (µç»ú2)
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  // PA1 - TIM5 CH2 (µç»ú1)
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	// ÅäÖÃTIM8µÄPWMÊä³öÍ¨µÀ
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  // PC8 - TIM8 CH3 (µç»ú4)
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  // PC9 - TIM8 CH4 (µç»ú3)
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	// Æô¶¯¶¨Ê±Æ÷
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
	// TIM8ÐèÒª¶îÍâµÄÖ÷Êä³öÊ¹ÄÜ
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

/**************************************************************************
¹ ¦ÄÜ£ºµç»ú·½Ïò¿ØÖÆÒý½Å³õÊ¼»¯
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Motor_Direction_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// Ê¹ÄÜÊ±ÖÓ
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);
	
	// ÅäÖÃ·½Ïò¿ØÖÆÒý½ÅÎªÍÆÍìÊä³ö
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// µç»ú1·½Ïò¿ØÖÆÒý½Å£ºPB0, PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// µç»ú2·½Ïò¿ØÖÆÒý½Å£ºPC4, PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// µç»ú3·½Ïò¿ØÖÆÒý½Å£ºPB5, PE14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// µç»ú4·½Ïò¿ØÖÆÒý½Å£ºPE4, PE5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**************************************************************************
¹¦ÄÜ£ºÉèÖÃÖ¸¶¨µç»úµÄ·½Ïò
Èë¿Ú²ÎÊý£ºµç»ú±àºÅ£¬·½Ïò£¨1Õý×ª£¬-1·´×ª£¬0Í£Ö¹£©
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Motor_SetDirection(MotorNum_t motor, int direction)
{
	switch(motor) {
		case Motor1:
			if (direction > 0) {
				// µç»ú1Õý×ª
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			} else if (direction < 0) {
				// µç»ú1·´×ª
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_SetBits(GPIOB, GPIO_Pin_1);
			} else {
				// µç»ú1Í£Ö¹
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			}
			break;
			
		case Motor2:
			if (direction > 0) {
				// µç»ú2Õý×ª
				GPIO_SetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			} else if (direction < 0) {
				// µç»ú2·´×ª
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_SetBits(GPIOC, GPIO_Pin_5);
			} else {
				// µç»ú2Í£Ö¹
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			}
			break;
			
		case Motor3:
			if (direction > 0) {
				// µç»ú3Õý×ª
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			} else if (direction < 0) {
				// µç»ú3·´×ª
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_SetBits(GPIOE, GPIO_Pin_14);
			} else {
				// µç»ú3Í£Ö¹
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			}
			break;
			
		case Motor4:
			if (direction < 0) {
				// µç»ú4Õý×ª
				GPIO_SetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			} else if (direction > 0) {
				// µç»ú4·´×ª
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_SetBits(GPIOE, GPIO_Pin_5);
			} else {
				// µç»ú4Í£Ö¹
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			}
			break;
	}
}

/**************************************************************************
¹¦ÄÜ£ºÉèÖÃÖ¸¶¨µç»úµÄPWMÖµ
Èë¿Ú²ÎÊý£ºµç»ú±àºÅ£¬PWMÖµ£¨0-100£©
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Motor_SetPWM(MotorNum_t motor, int pwm)
{
	// ÏÞÖÆPWMÖµ·¶Î§
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
¹¦ÄÜ£ºÍ£Ö¹ËùÓÐµç»ú
Èë¿Ú²ÎÊý£ºÎÞ
·µ»Ø  Öµ£ºÎÞ
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
¹¦ÄÜ£ºÉèÖÃµç»úPWMºÍ·½Ïò
Èë¿Ú²ÎÊý£ºµç»ú±àºÅ£¬PWMÖµ£¨-100µ½100£©
·µ»Ø  Öµ£ºÎÞ
**************************************************************************/
void Set_Motor_PWM(MotorNum_t motor, int pwm)
{
	if(motor >= 4) return; // ²ÎÊý¼ì²é
	
	#define Dead_Voltage 10      // ËÀÇø²¹³¥Öµ
    #define Amplitude 90        // PWM×î´ó·ùÖµ
	
	if(pwm > 0) {
		// Õý×ª
		Motor_SetDirection(motor, 1);
		pwm = pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else if(pwm < 0) {
		// ·´×ª
		Motor_SetDirection(motor, -1);
		pwm = -pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else {
		// Í£Ö¹
		Motor_SetDirection(motor, 0);
		Motor_SetPWM(motor, 0);
	}
}
