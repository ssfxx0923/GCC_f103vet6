#include "TB6612.h"
#include "encoder.h"
#include "pid_control.h"



/**************************************************************************
功能：TB6612初始化函数
入口参数：无
返回  值：无
**************************************************************************/
void MOTOR_Init(void)
{
	// 初始化硬件PWM
	Hardware_PWM_Init();
	// 初始化方向控制引脚
	Motor_Direction_Init();
	// 停止所有电机
	Move_stop();
}

/**************************************************************************
功能：硬件PWM初始化
入口参数：无
返回  值：无
说明：使用TIM5输出PWM到PA0(电机2)和PA1(电机1)
     使用TIM8输出PWM到PC8(电机4)和PC9(电机3)
**************************************************************************/
void Hardware_PWM_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	// 使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM5, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);
	
	// 配置GPIO为复用推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// 配置PA0 (电机2) 和 PA1 (电机1) 为TIM5的PWM输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	// 配置PC8 (电机4) 和 PC9 (电机3) 为TIM8的PWM输出
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// 配置TIM5时基单元
	TIM_TimeBaseStructure.TIM_Period = 99;                      // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // 预分频值，不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInit(TIM5, &TIM_TimeBaseStructure);
	
	// 配置TIM8时基单元
	TIM_TimeBaseStructure.TIM_Period = 99;                      // 自动重装载值
	TIM_TimeBaseStructure.TIM_Prescaler = 0;                    // 预分频值，不分频
	TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;     // 时钟分割
	TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up; // 向上计数模式
	TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);
	
	// 配置PWM输出比较结构体
	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;           // PWM模式1
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; // 输出使能
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;   // 输出极性高
	TIM_OCInitStructure.TIM_Pulse = 0;                          // 初始占空比为0
	
	// 配置TIM5的PWM输出通道
	TIM_OC1Init(TIM5, &TIM_OCInitStructure);  // PA0 - TIM5 CH1 (电机2)
	TIM_OC1PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	TIM_OC2Init(TIM5, &TIM_OCInitStructure);  // PA1 - TIM5 CH2 (电机1)
	TIM_OC2PreloadConfig(TIM5, TIM_OCPreload_Enable);
	
	// 配置TIM8的PWM输出通道
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
	TIM_OCInitStructure.TIM_OutputNState = TIM_OutputNState_Disable;
	
	TIM_OC3Init(TIM8, &TIM_OCInitStructure);  // PC8 - TIM8 CH3 (电机4)
	TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	TIM_OC4Init(TIM8, &TIM_OCInitStructure);  // PC9 - TIM8 CH4 (电机3)
	TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	// 启动定时器
	TIM_Cmd(TIM5, ENABLE);
	TIM_Cmd(TIM8, ENABLE);
	
	// TIM8需要额外的主输出使能
	TIM_CtrlPWMOutputs(TIM8, ENABLE);
}

/**************************************************************************
功能：电机方向控制引脚初始化
入口参数：无
返回  值：无
**************************************************************************/
void Motor_Direction_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// 使能时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOE, ENABLE);
	
	// 配置方向控制引脚为推挽输出
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	
	// 电机1方向控制引脚：PB0, PB1
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	// 电机2方向控制引脚：PC4, PC5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOC, &GPIO_InitStructure);
	
	// 电机3方向控制引脚：PB5, PE14
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	// 电机4方向控制引脚：PE4, PE5
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
}

/**************************************************************************
功能：设置指定电机的方向
入口参数：电机编号，方向（1正转，-1反转，0停止）
返回  值：无
**************************************************************************/
void Motor_SetDirection(MotorNum_t motor, int direction)
{
	switch(motor) {
		case Motor1:
			if (direction > 0) {
				// 电机1正转
				GPIO_SetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			} else if (direction < 0) {
				// 电机1反转
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_SetBits(GPIOB, GPIO_Pin_1);
			} else {
				// 电机1停止
				GPIO_ResetBits(GPIOB, GPIO_Pin_0);
				GPIO_ResetBits(GPIOB, GPIO_Pin_1);
			}
			break;
			
		case Motor2:
			if (direction > 0) {
				// 电机2正转
				GPIO_SetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			} else if (direction < 0) {
				// 电机2反转
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_SetBits(GPIOC, GPIO_Pin_5);
			} else {
				// 电机2停止
				GPIO_ResetBits(GPIOC, GPIO_Pin_4);
				GPIO_ResetBits(GPIOC, GPIO_Pin_5);
			}
			break;
			
		case Motor3:
			if (direction > 0) {
				// 电机3正转
				GPIO_SetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			} else if (direction < 0) {
				// 电机3反转
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_SetBits(GPIOE, GPIO_Pin_14);
			} else {
				// 电机3停止
				GPIO_ResetBits(GPIOB, GPIO_Pin_5);
				GPIO_ResetBits(GPIOE, GPIO_Pin_14);
			}
			break;
			
		case Motor4:
			if (direction < 0) {
				// 电机4正转
				GPIO_SetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			} else if (direction > 0) {
				// 电机4反转
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_SetBits(GPIOE, GPIO_Pin_5);
			} else {
				// 电机4停止
				GPIO_ResetBits(GPIOE, GPIO_Pin_4);
				GPIO_ResetBits(GPIOE, GPIO_Pin_5);
			}
			break;
	}
}

/**************************************************************************
功能：设置指定电机的PWM值
入口参数：电机编号，PWM值（0-100）
返回  值：无
**************************************************************************/
void Motor_SetPWM(MotorNum_t motor, int pwm)
{
	// 限制PWM值范围
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
功能：停止所有电机
入口参数：无
返回  值：无
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
功能：设置电机PWM和方向（包含死区补偿）
入口参数：电机编号，PWM值（-100到100）
返回  值：无
**************************************************************************/
void Set_Motor_PWM(MotorNum_t motor, int pwm)
{
	if(motor >= 4) return; // 参数检查
	
	#define Dead_Voltage 10      // 死区补偿值
    #define Amplitude 90        // PWM最大幅值
	
	if(pwm > 0) {
		// 正转
		Motor_SetDirection(motor, 1);
		pwm = pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else if(pwm < 0) {
		// 反转
		Motor_SetDirection(motor, -1);
		pwm = -pwm + Dead_Voltage;
		if(pwm > Amplitude) pwm = Amplitude;
		Motor_SetPWM(motor, pwm);
	} else {
		// 停止
		Motor_SetDirection(motor, 0);
		Motor_SetPWM(motor, 0);
	}
}

/**************************************************************************
功能：限幅函数
入口参数：输入数据，最大值
返回  值：限幅后的数据
**************************************************************************/
int Xianfu(int data, int max)
{
	if(data < -max) data = -max;
	if(data > max) data = max;
	return data;
}

