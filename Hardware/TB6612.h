#ifndef __TB6612_H 
#define __TB6612_H 
#include "sys.h"
#include "encoder.h"
#include "pid_control.h"

// PWM相关宏定义
#define PWM_MAX_VALUE 99     // PWM最大值 (0-99)，匹配TIM8的ARR设置

// 电机驱动函数声明
void MOTOR_Init(void);                                      // 电机初始化
void Hardware_PWM_Init(void);                               // 硬件PWM初始化
void Motor_Direction_Init(void);                            // 方向控制引脚初始化
void Move_stop(void);                                       // 停止所有电机

// 电机硬件控制函数声明
void Motor_SetDirection(MotorNum_t motor, int direction);   // 设置电机方向
void Motor_SetPWM(MotorNum_t motor, int pwm);              // 设置电机PWM值

// 串级PID控制相关电机函数声明
void Set_Motor_PWM(MotorNum_t motor, int pwm);            // 设置PWM和方向（含死区补偿）(-100到100)
int Xianfu(int data, int max);                             // 限幅函数

#endif
