#include "pid_control.h"
#include "TB6612.h"
#include "delay.h"
#include "OLED.h"
#include "Usart5.h"
// 串级PID参数 - 位置环（外环）
float Position_KP = 0.7f, Position_KI = 0.0010f, Position_KD = 0.1f;

// 串级PID参数 - 速度环（内环） 
float Incremental_KP = 2.5f, Incremental_KI = 0.0f, Incremental_KD = 0.6f;

// 电机控制状态
static Motor_Control_State_t motor_states[4];

// PID控制使能标志
static uint8_t pid_control_enabled = 0;

/**************************************************************************
功能：初始化PID控制模块
入口参数：无
返回  值：无
说明：初始化TIM6为1kHz中断，用于PID计算
**************************************************************************/
void PID_Control_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // 使能TIM6时钟
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    // 配置TIM6为1kHz中断频率 (36MHz/(35+1)/(999+1) = 1kHz)
    TIM_TimeBaseInitStrue.TIM_Period = 999;          // 自动重装值999，产生1000个计数步
    TIM_TimeBaseInitStrue.TIM_Prescaler = 35;        // 预分频35，TIM6是APB1时钟(36MHz)
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStrue);

    // 配置TIM6中断
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // 配置NVIC中断优先级
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // 高优先级，确保PID计算及时执行
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // 使能定时器
    TIM_Cmd(TIM6, ENABLE);
}

/**************************************************************************
功能：TIM6中断服务程序 - PID控制计算
入口参数：无
返回  值：无
说明：每1kHz调用一次，执行四路PID控制计算和编码器位置累积
**************************************************************************/
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        // 清除中断标志
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        
        // 串级PID控制处理
        if (pid_control_enabled) {
            // 四路并发串级PID控制计算
            for (int i = 0; i < 4; i++) {
                if (motor_states[i].control_enabled) {
                    static int Moto[4] = {0};
                    
                    // 获取实际速度
                    int Reality_Velocity = (int)Get_Motor_Speed((MotorNum_t)i);
                    // 获取并更新实际位置
                    int Reality_Position = (int)Get_Accumulated_Position((MotorNum_t)i); 
                    
                    // 位置式PID控制（外环）- 输出目标速度
                    Moto[i] = Position_PID(Reality_Position, (int)motor_states[i].target_position, i);
                    
                    // 位置环输出限幅到目标速度范围
                    Moto[i] = Xianfu(Moto[i], (int)motor_states[i].target_speed);
                    
                    // 检查是否到达目标位置（误差容忍度检查）
                    if ((Reality_Position - (int)motor_states[i].target_position) < 1 && (Reality_Position - (int)motor_states[i].target_position) > -1) {
                        Incremental_PID(0, 0, i);  // 清零增量式PID累积状态
                        Set_Motor_PWM((MotorNum_t)i, 0);
                        motor_states[i].finish_flag = 1;
                    } else {
                        // 增量式PID控制（内环）- 速度环控制
                        Moto[i] = Incremental_PID(Reality_Velocity, Moto[i], i);
                        
                        // 设置PWM输出
                        Set_Motor_PWM((MotorNum_t)i, Moto[i]);
                    }
                }
            }
        }
    }
}

/**************************************************************************
功能：获取累积位置并计算速度
入口参数：电机编号
返回  值：累积位置值
说明：实现编码器脉冲累积，避免16位计数器回绕问题
**************************************************************************/
int32_t Get_Accumulated_Position(MotorNum_t motor)
{
    Motor_Control_State_t* state = &motor_states[motor];
    
    // 获取当前编码器计数值
    int16_t current_count = Get_Encoder_Count(motor);
    
    // 计算瞬时速度：当前CNT值就是1ms内的脉冲数，转换为脉冲/秒
    state->current_speed = (float)current_count * SPEED_CALC_FREQUENCY_HZ;
    
    // 将当前CNT值直接累加到累积位置
    state->accumulated_position += current_count;
    
    // 获取后立即清零编码器计数器
    Reset_Encoder(motor);
    
    return state->accumulated_position;
}

/**************************************************************************
功能：获取电机原始速度
入口参数：电机编号
返回  值：原始速度值（编码器脉冲/秒）
说明：返回未滤波的瞬时速度，响应快
**************************************************************************/
float Get_Motor_Speed(MotorNum_t motor)
{
    if (motor >= 4) return 0.0f; // 参数检查
    
    return motor_states[motor].current_speed;
}


/**************************************************************************
功能：重置累积位置
入口参数：电机编号
返回  值：无
说明：重置指定电机的累积位置
**************************************************************************/
void Reset_Accumulated_Position(MotorNum_t motor)
{
    Motor_Control_State_t* state = &motor_states[motor];
    
    // 重置编码器硬件计数器
    Reset_Encoder(motor);
    
    // 重置累积状态
    state->accumulated_position = 0;
    state->current_speed = 0.0f;
}

/**************************************************************************
功能：串级PID控制器初始化
入口参数：无
返回  值：无
说明：初始化串级PID控制模块
**************************************************************************/
void Motor_Cascade_PID_Init(void)
{
    int i;
    
    // 初始化PID控制模块（TIM6）
    PID_Control_Init();
    
    // 初始化四路电机状态
    for (i = 0; i < 4; i++) {

        // 设置默认速度限制
        motor_states[i].target_speed = 0.0f;  // 设置合理的默认速度限制
        
        // 重置累积位置和速度状态
        Reset_Accumulated_Position((MotorNum_t)i);
    }
    
    // 停止所有电机
    Move_stop();
    
    // 禁用PID控制
    pid_control_enabled = 0;
}

/**************************************************************************
功能：设置单个电机目标位置
入口参数：电机编号，转动圈数（正数正转，负数反转）
返回  值：无
说明：设置指定电机的目标位置，但不启动控制
**************************************************************************/
void Motor_Set_Target(MotorNum_t motor, float revolutions)
{
    if (motor >= 4) return; // 参数检查
    
    // 计算目标编码器计数值
    motor_states[motor].target_position = revolutions * (ENCODER_COUNTS_PER_REVOLUTION-7.45);
    motor_states[motor].control_enabled = 1; // 使能该电机控制
    
    // 重置对应累积位置
    Reset_Accumulated_Position(motor);
    
}

/**************************************************************************
功能：设置单个电机目标位置和速度限制
入口参数：电机编号，转动圈数，最大速度限制
返回  值：无
说明：串级控制专用接口，支持速度限制的位置控制
**************************************************************************/
void Motor_Set_Target_With_Speed(MotorNum_t motor, float revolutions, float max_speed)
{
    if (motor >= 4) return; // 参数检查
    
    // 设置目标位置
    Motor_Set_Target(motor, revolutions);
    
    // 设置速度限制
    if(max_speed > 0) {
        motor_states[motor].target_speed = max_speed * (ENCODER_COUNTS_PER_REVOLUTION-7.45);
    } else {
        motor_states[motor].target_speed = 0.0f; 
    }

    Motor_Start_Control();
}

/**************************************************************************
功能：启动PID控制
入口参数：无
返回  值：无
说明：启动中断驱动的四路并发PID控制
**************************************************************************/
void Motor_Start_Control(void)
{
    // 启用PID控制
    pid_control_enabled = 1;
}

/**************************************************************************
功能：停止PID控制
入口参数：无
返回  值：无
说明：停止所有PID控制并停止所有电机
**************************************************************************/
void Motor_Stop_Control(void)
{
    int i;
    
    // 禁用PID控制
    pid_control_enabled = 0;
    
    // 停止所有电机
    Move_stop();
    
    // 禁用所有电机控制
    for (i = 0; i < 4; i++) {
        motor_states[i].control_enabled = 0;
    }
}



/**************************************************************************
功能：停止所有电机但保持锁死状态
入口参数：无
返回  值：无
说明：停止所有电机
**************************************************************************/
void Stop_All_Motor(void)
{
    Motor_Set_Target_With_Speed(Motor1, 0, 100);    
    Motor_Set_Target_With_Speed(Motor2, 0, 100);    
    Motor_Set_Target_With_Speed(Motor3, 0, 100);    
    Motor_Set_Target_With_Speed(Motor4, 0, 100);    
}

/**************************************************************************
功能：位置式PID控制器
入口参数：实际位置，目标位置，电机编号
返回  值：输出PWM（目标速度）
说明：位置式离散PID公式 
pwm=Kp*e(k)+Ki*∑e(k)+Kd[e(k)-e(k-1)]
e(k)：当前误差偏差 
e(k-1)：上一次的偏差  
∑e(k)：误差e(k)以及之前的偏差的累积和;即∑为k=1,2,...,k;
pwm：输出结果
**************************************************************************/
int Position_PID(int reality, int target, int motor_index)
{ 	
    static float Bias[4], Pwm[4], Last_Bias[4], Integral_bias[4] = {0};
    
    if(motor_index >= 4) return 0; // 参数检查
    
    Bias[motor_index] = target - reality;                    // 计算偏差
    Integral_bias[motor_index] += Bias[motor_index];	     // 偏差累积
    
    if(Integral_bias[motor_index] > 5000) Integral_bias[motor_index] = 5000;   // 积分限幅
    if(Integral_bias[motor_index] < -5000) Integral_bias[motor_index] = -5000;
    
    Pwm[motor_index] = (Position_KP * Bias[motor_index])                        // 比例项输出
                     + (Position_KI * Integral_bias[motor_index])               // 积分项输出
                     + (Position_KD * (Bias[motor_index] - Last_Bias[motor_index])); // 微分项输出
    
    Last_Bias[motor_index] = Bias[motor_index];                                 // 保存上次偏差
    
    return (int)Pwm[motor_index];                                               // 返回输出
}

/**************************************************************************
功能：增量式PID控制器
入口参数：实际值，目标值，电机编号
返回  值：输出PWM
说明：增量式离散PID公式 
pwm+=Kp[e(k)-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)：当前误差偏差 
e(k-1)：上一次的偏差  此次用于速度控制 
pwm：输出增量结果
**************************************************************************/
int Incremental_PID(int reality, int target, int motor_index)
{ 	
    static float Bias[4], Pwm[4] = {0}, Last_bias[4] = {0}, Prev_bias[4] = {0};
    
    if(motor_index >= 4) return 0; // 参数检查
    
    Bias[motor_index] = target - reality;                                       // 计算偏差
    
    Pwm[motor_index] += (Incremental_KP * (Bias[motor_index] - Last_bias[motor_index]))        // 比例项输出
                      + (Incremental_KI * Bias[motor_index])                                   // 积分项输出
                      + (Incremental_KD * (Bias[motor_index] - 2*Last_bias[motor_index] + Prev_bias[motor_index])); // 微分项输出
    
    Prev_bias[motor_index] = Last_bias[motor_index];                            // 保存上上次偏差
    Last_bias[motor_index] = Bias[motor_index];	                                // 保存上一次偏差
    
    return (int)Pwm[motor_index];                                               // 返回输出
}



void TurnGo(float circle, float speed)
{
    Motor_Set_Target_With_Speed(Motor1, circle, speed);
    Motor_Set_Target_With_Speed(Motor2,-circle, speed);
    Motor_Set_Target_With_Speed(Motor3, circle, speed);
    Motor_Set_Target_With_Speed(Motor4, -circle, speed);
     while(motor_states[0].finish_flag == 0 && motor_states[1].finish_flag == 0 && motor_states[2].finish_flag == 0 && motor_states[3].finish_flag == 0);
    Stop_All_Motor();
}