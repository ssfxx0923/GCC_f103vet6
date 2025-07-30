#ifndef __PID_CONTROL_H
#define __PID_CONTROL_H

#include "sys.h"
#include "encoder.h"

#define CONTROL_FREQUENCY_HZ        1000        // PID控制频率1kHz
#define POSITION_TOLERANCE          2           // 位置控制误差容忍度
#define POSITION_INTEGRAL_LIMIT     5000.0f     // 位置环积分限幅
#define SPEED_OUTPUT_LIMIT          100.0f       // 速度环输出限幅
#define SPEED_INTEGRAL_LIMIT        100.0f      // 速度环积分限幅

typedef enum {
    CONTROL_SPEED = 0,      // 速度控制模式
    CONTROL_POSITION        // 位置控制模式
} Motor_Control_Mode_t;

// 电机控制状态结构体
typedef struct {
    Motor_Control_Mode_t control_mode;  // 控制模式 
    float target_position;              // 目标位置(编码器脉冲)
    float target_speed;                 // 目标速度(RPM)
    float max_speed;                    // 最大允许速度(RPM)
    int32_t accumulated_position;       // 累积位置
    float current_speed;                // 当前速度(RPM)
} Motor_Control_State_t;

void PID_Control_Init(void);                                           // PID控制模块初始化
void Position_Control_Start(MotorNum_t motor, float revolutions, float max_speed_rpm);  // 启动位置控制
void Speed_Control_Start(MotorNum_t motor, float rpm);                 // 启动速度控制
void Motor_Control_Stop(MotorNum_t motor);                             // 停止单个电机控制
void Motor_Control_Stop_All(void);                                     // 停止所有电机控制
void Reset_Motor_State(MotorNum_t motor);                              // 重置电机状态
int32_t Get_Accumulated_Position(MotorNum_t motor);                     // 获取累积位置
void Reset_Accumulated_Position(MotorNum_t motor);                      // 重置累积位置
float Get_Motor_Speed(MotorNum_t motor);                               // 获取电机速度(RPM)
float Position_PID_RPM(int current_position, int target_position, int motor_index);         // 位置环PID(输出RPM)
float Speed_PID_Calculate(float current_speed_rpm, float target_speed_rpm, int motor_index,
                         float KP, float KI, float KD);                                    // 速度环PID(RPM输入，可配置参数)
void Reset_Speed_PID(MotorNum_t motor);                                                     // 重置速度PID状态
void Position_Control_Start_All(float revolutions, float max_speed_rpm);
float Limit_Value(float value, float limit);     

#endif 
