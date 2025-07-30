#include "pid_control.h"
#include "TB6612.h"
#include "delay.h"
#include "OLED.h"
#include "Usart5.h"
#include <stdlib.h>

// λ�û��������⻷��
float Position_KP = 4.7f, Position_KI = 0.02f, Position_KD = 0.3f;
// ���������е��ٶȻ��������ڻ���
float Cascade_Speed_KP = 4.5f, Cascade_Speed_KI = 0.0f, Cascade_Speed_KD = 0.15f;

// �����ٶȿ��Ʋ���
float Direct_Speed_KP = 5.0f, Direct_Speed_KI = 0.02f, Direct_Speed_KD = 0.5f;

// ���״̬����
static Motor_Control_State_t motor_states[4];

/**************************************************************************
���ܣ�PID����ģ���ʼ��
��ڲ�������
����  ֵ����
˵������ʼ��TIM6Ϊ1kHz�жϣ�����PID����
**************************************************************************/
void PID_Control_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    NVIC_InitTypeDef NVIC_InitStructure;
    int i;
    
    // ʹ��TIM6ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    // ����TIM6Ϊ1kHz�ж�Ƶ�� (36MHz/(35+1)/(999+1) = 1kHz)
    TIM_TimeBaseInitStrue.TIM_Period = 999;          
    TIM_TimeBaseInitStrue.TIM_Prescaler = 35;        
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStrue);

    // ����TIM6�ж�
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // ����NVIC�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; 
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ��ʼ�����е��״̬
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
���ܣ�TIM6�жϷ������ - PID���Ƽ���
��ڲ�������
����  ֵ����
˵�������ݿ���ģʽִ����Ӧ��PID����
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
���ܣ�����λ�ÿ���
��ڲ����������ţ�Ŀ��Ȧ��������ٶ�(RPM)
����  ֵ����
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
���ܣ������ٶȿ���
��ڲ����������ţ�Ŀ���ٶ�(RPM)
����  ֵ����
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
���ܣ�ֹͣ�������
��ڲ�����������
����  ֵ����
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
���ܣ�ֹͣ���е������
��ڲ�������
����  ֵ����
**************************************************************************/
void Motor_Control_Stop_All(void)
{
    for (int i = 0; i < 4; i++) {
        Motor_Control_Stop((MotorNum_t)i);
    }
}

/**************************************************************************
���ܣ����õ��״̬
��ڲ�����������
����  ֵ����
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
���ܣ���ȡ�ۻ�λ�ò������ٶ�
��ڲ�����������
����  ֵ���ۻ�λ��ֵ
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
���ܣ���ȡ����ٶ�
��ڲ�����������
����  ֵ���ٶ�ֵ��RPM��
**************************************************************************/
float Get_Motor_Speed(MotorNum_t motor)
{
    if (motor >= 4) return 0.0f;
    return motor_states[motor].current_speed;
}

/**************************************************************************
���ܣ������ۻ�λ��
��ڲ�����������
����  ֵ����
**************************************************************************/
void Reset_Accumulated_Position(MotorNum_t motor)
{
    if (motor >= 4) return;
    
    Reset_Encoder(motor);
    motor_states[motor].accumulated_position = 0;
    motor_states[motor].current_speed = 0.0f;
}

/**************************************************************************
���ܣ�λ�û�PID�����������RPM��
��ڲ�������ǰλ�ã�Ŀ��λ�ã�������
����  ֵ��Ŀ���ٶȣ�RPM��
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
���ܣ�λ��ʽ�ٶ�PID��������RPM���룩
��ڲ�������ǰ�ٶ�(RPM)��Ŀ���ٶ�(RPM)��������
����  ֵ��PWM���ֵ
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
���ܣ������ٶ�PID������״̬
��ڲ�����������
����  ֵ����
**************************************************************************/
void Reset_Speed_PID(MotorNum_t motor)
{
    Speed_PID_Calculate(0.0f, 0.0f, motor, -1.0f, -1.0f, -1.0f);
}


/**************************************************************************
���ܣ��޷�����
��ڲ���������ֵ������ֵ
����  ֵ���޷����ֵ
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