#ifndef __PID_CONTROL_H
#define __PID_CONTROL_H

#include "sys.h"
#include "encoder.h"

// PID控制相关宏定义
#define SPEED_CALC_FREQUENCY_HZ 1000     // 速度计算频率1kHz

// 电机控制状态结构体
typedef struct {
    float target_position;          // 目标位置(编码器计数)
    float target_speed;             // 目标速度(编码器脉冲/秒)
    int32_t accumulated_position;   // 累积位置
    float current_speed;            // 当前速度(编码器脉冲/秒)
    uint8_t finish_flag;            //运动完成标志
    uint8_t control_enabled;        // 控制使能标志
} Motor_Control_State_t;

// PID控制相关函数声明
void PID_Control_Init(void);                                       // 初始化PID控制模块

// 串级PID控制函数声明
void Motor_Cascade_PID_Init(void);                           // 初始化串级PID控制器
void Motor_Set_Target(MotorNum_t motor, float revolutions);   // 设置单电机目标位置
void Motor_Start_Control(void);                               // 启动PID控制
void Motor_Stop_Control(void);                                // 停止PID控制
void Motor_Set_Target_With_Speed(MotorNum_t motor, float revolutions, float max_speed); // 设置位置和速度限制
void TurnGo(float circle, float speed);
void Stop_All_Motor(void);
// 编码器位置累积函数声明
int32_t Get_Accumulated_Position(MotorNum_t motor);                // 获取累积位置
void Reset_Accumulated_Position(MotorNum_t motor);                 // 重置累积位置
float Get_Motor_Speed(MotorNum_t motor);                          // 获取原始速度


int Position_PID(int reality, int target, int motor_index);    // 位置环PID
int Incremental_PID(int reality, int target, int motor_index); // 速度环PID
#endif 
