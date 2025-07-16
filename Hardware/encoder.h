#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"

// 编码器相关宏定义
#define ENCODER_COUNTS_PER_REVOLUTION 1560  // 电机转一圈编码器计数值

// 电机编号定义
typedef enum {
    Motor1 = 0,  // 对应TIM1编码器
    Motor2 = 1,  // 对应TIM2编码器
    Motor3 = 2,  // 对应TIM3编码器
    Motor4 = 3   // 对应TIM4编码器
} MotorNum_t;


// 多编码器函数声明
void Encoder_Init(void);                          // 初始化所有编码器
int16_t Get_Encoder_Count(MotorNum_t motor);      // 获取指定编码器计数
void Reset_Encoder(MotorNum_t motor);             // 重置指定编码器计数
void Reset_All_Encoders(void);                    // 重置所有编码器计数

#endif
