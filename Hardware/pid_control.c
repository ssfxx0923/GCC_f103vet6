#include "pid_control.h"
#include "TB6612.h"
#include "delay.h"
#include "OLED.h"
#include "Usart5.h"
#include <stdlib.h>

// 位置环参数（外环）
float Position_KP = 4.7f, Position_KI = 0.02f, Position_KD = 0.3f;
// 串级控制中的速度环参数（内环）
float Cascade_Speed_KP = 4.5f, Cascade_Speed_KI = 0.0f, Cascade_Speed_KD = 0.15f;

// 独立速度控制参数
float Direct_Speed_KP = 5.0f, Direct_Speed_KI = 0.02f, Direct_Speed_KD = 0.5f;

// 电机状态管理
static Motor_Control_State_t motor_states[4];

/**************************************************************************
功能：PID控制模块初始化
入口参数：无
返回  值：无
说明：初始化TIM6为1kHz中断，用于PID计算
**************************************************************************/
void PID_Control_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    NVIC_InitTypeDef NVIC_InitStructure;
    int i;
    
    // 使能TIM6时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    // 配置TIM6为1kHz中断频率 (36MHz/(35+1)/(999+1) = 1kHz)
    TIM_TimeBaseInitStrue.TIM_Period = 999;          
    TIM_TimeBaseInitStrue.TIM_Prescaler = 35;        
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStrue);

    // 配置TIM6中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // 配置NVIC中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 初始化所有电机状态
    for (i = 0; i < 4; i++) {
        Reset_Motor_State((MotorNum_t)i);
        motor_states[i].control_mode = CONTROL_POSITION;
        motor_states[i].target_position = 0;  
        motor_states[i].max_speed = 50.0f;   
    }
    
    Move_stop();
    
    TIM_Cmd(TIM6, ENABLE);
}

/**************************************************************************
功能：TIM6中断服务程序 - PID控制计算
入口参数：无
返回  值：无
说明：根据控制模式执行相应的PID控制
**************************************************************************/
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        
        for (int i = 0; i < 4; i++) {
            Motor_Control_State_t* state = &motor_states[i];
            
            switch (state->control_mode) {
                case CONTROL_POSITION: {
                    int current_position = (int)Get_Accumulated_Position((MotorNum_t)i);
                    float current_speed_rpm = Get_Motor_Speed((MotorNum_t)i);
                    
                    float target_speed_rpm = Position_PID_RPM(current_position, (int)state->target_position, i);
                    
                    target_speed_rpm = Limit_Value(target_speed_rpm, state->max_speed);
                    
                    if (abs(current_position - (int)state->target_position) <= POSITION_TOLERANCE) {
                        Set_Motor_PWM((MotorNum_t)i, 0);
                    } else {
                        float pwm_output = Speed_PID_Calculate(current_speed_rpm, target_speed_rpm, i, 
                                                             Cascade_Speed_KP, Cascade_Speed_KI, Cascade_Speed_KD);
                        Set_Motor_PWM((MotorNum_t)i, (int)pwm_output);
                    }
                    break;
                }
                
                case CONTROL_SPEED: {
                    int current_position = (int)Get_Accumulated_Position((MotorNum_t)i); 
                    float current_speed_rpm = Get_Motor_Speed((MotorNum_t)i);
                    
                    float pwm_output = Speed_PID_Calculate(current_speed_rpm, state->target_speed, i,
                                                         Direct_Speed_KP, Direct_Speed_KI, Direct_Speed_KD);
                    Set_Motor_PWM((MotorNum_t)i, (int)pwm_output);
                    break;
                }
                
            }
        }
    }
}

/**************************************************************************
功能：启动位置控制
入口参数：电机编号，目标圈数，最大速度(RPM)
返回  值：无
**************************************************************************/
void Position_Control_Start(MotorNum_t motor, float revolutions, float max_speed_rpm)
{
    if (motor >= 4) return;
    
    Motor_Control_State_t* state = &motor_states[motor];
    
    state->control_mode = CONTROL_POSITION;
    state->target_position = revolutions * (ENCODER_COUNTS_PER_REVOLUTION-12);
    state->max_speed = max_speed_rpm; 
    
    Reset_Accumulated_Position(motor);
}

/**************************************************************************
功能：启动速度控制
入口参数：电机编号，目标速度(RPM)
返回  值：无
**************************************************************************/
void Speed_Control_Start(MotorNum_t motor, float rpm)
{
    if (motor >= 4) return;

    Motor_Control_State_t* state = &motor_states[motor];

    state->control_mode = CONTROL_SPEED;
    state->target_speed = rpm;

    Reset_Encoder(motor);
    Reset_Speed_PID(motor);
}

/**************************************************************************
功能：停止电机控制
入口参数：电机编号
返回  值：无
**************************************************************************/
void Motor_Control_Stop(MotorNum_t motor)
{
    if (motor >= 4) return;
    
    motor_states[motor].control_mode = CONTROL_POSITION;
    motor_states[motor].target_position = motor_states[motor].accumulated_position;
    motor_states[motor].max_speed = 50.0f;  
    Set_Motor_PWM(motor, 0);
}

/**************************************************************************
功能：停止所有电机控制
入口参数：无
返回  值：无
**************************************************************************/
void Motor_Control_Stop_All(void)
{
    for (int i = 0; i < 4; i++) {
        Motor_Control_Stop((MotorNum_t)i);
    }
}

/**************************************************************************
功能：重置电机状态
入口参数：电机编号
返回  值：无
**************************************************************************/
void Reset_Motor_State(MotorNum_t motor)
{
    if (motor >= 4) return;
    
    Motor_Control_State_t* state = &motor_states[motor];
    
    state->control_mode = CONTROL_POSITION;  
    state->target_position = 0;
    state->target_speed = 0;
    state->max_speed = 50.0f;               
    state->accumulated_position = 0;
    state->current_speed = 0;
    
    Reset_Encoder(motor);
}

/**************************************************************************
功能：获取累积位置并计算速度
入口参数：电机编号
返回  值：累积位置值
**************************************************************************/
int32_t Get_Accumulated_Position(MotorNum_t motor)
{
    if (motor >= 4) return 0;
    
    Motor_Control_State_t* state = &motor_states[motor];
    
    int16_t current_count = Get_Encoder_Count(motor);
    
    state->current_speed = (float)current_count * 60.0f / ENCODER_COUNTS_PER_REVOLUTION * CONTROL_FREQUENCY_HZ;
    
    state->accumulated_position += current_count;
    
    Reset_Encoder(motor);
    
    return state->accumulated_position;
}

/**************************************************************************
功能：获取电机速度
入口参数：电机编号
返回  值：速度值（RPM）
**************************************************************************/
float Get_Motor_Speed(MotorNum_t motor)
{
    if (motor >= 4) return 0.0f;
    return motor_states[motor].current_speed;
}

/**************************************************************************
功能：重置累积位置
入口参数：电机编号
返回  值：无
**************************************************************************/
void Reset_Accumulated_Position(MotorNum_t motor)
{
    if (motor >= 4) return;
    
    Reset_Encoder(motor);
    motor_states[motor].accumulated_position = 0;
    motor_states[motor].current_speed = 0.0f;
}

/**************************************************************************
功能：位置环PID控制器（输出RPM）
入口参数：当前位置，目标位置，电机编号
返回  值：目标速度（RPM）
**************************************************************************/
float Position_PID_RPM(int current_position, int target_position, int motor_index)
{
    static float bias[4], integral[4] = {0}, last_bias[4] = {0};
    
    if (motor_index >= 4) return 0;
    
    bias[motor_index] = target_position - current_position;
    
    integral[motor_index] += bias[motor_index];
    
    integral[motor_index] = Limit_Value(integral[motor_index], POSITION_INTEGRAL_LIMIT);
    
    float output = Position_KP * bias[motor_index]
                 + Position_KI * integral[motor_index]
                 + Position_KD * (bias[motor_index] - last_bias[motor_index]);
    
    float output_rpm = output * 60.0f / ENCODER_COUNTS_PER_REVOLUTION;
    
    last_bias[motor_index] = bias[motor_index];
    
    return output_rpm;
}

/**************************************************************************
功能：位置式速度PID控制器（RPM输入）
入口参数：当前速度(RPM)，目标速度(RPM)，电机编号
返回  值：PWM输出值
**************************************************************************/
float Speed_PID_Calculate(float current_speed_rpm, float target_speed_rpm, int motor_index, float KP, float KI, float KD)
{
    static float integral[4] = {0}, last_error[4] = {0};

    if (motor_index >= 4) return 0;

    if (KP == -1 && KI == -1 && KD == -1) {
        integral[motor_index] = 0;
        last_error[motor_index] = 0;
        return 0;
    }

    float current_error = target_speed_rpm - current_speed_rpm;

    integral[motor_index] += current_error;

    integral[motor_index] = Limit_Value(integral[motor_index], SPEED_INTEGRAL_LIMIT);

    float derivative = current_error - last_error[motor_index];

    float output = KP * current_error +
                  KI * integral[motor_index] +
                  KD * derivative;

    output = Limit_Value(output, SPEED_OUTPUT_LIMIT);

    last_error[motor_index] = current_error;

    return output;
}

/**************************************************************************
功能：重置速度PID控制器状态
入口参数：电机编号
返回  值：无
**************************************************************************/
void Reset_Speed_PID(MotorNum_t motor)
{
    Speed_PID_Calculate(0.0f, 0.0f, motor, -1.0f, -1.0f, -1.0f);
}


/**************************************************************************
功能：限幅函数
入口参数：输入值，限制值
返回  值：限幅后的值
**************************************************************************/
float Limit_Value(float value, float limit)
{
    if (value > limit) return limit;
    if (value < -limit) return -limit;
    return value;
}

void Position_Control_Start_All(float revolutions, float max_speed_rpm)
{
    for (int i = 0; i < 4; i++) {
        Position_Control_Start((MotorNum_t)i, revolutions, max_speed_rpm);
    }
}