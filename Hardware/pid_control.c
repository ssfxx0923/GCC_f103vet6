#include "pid_control.h"
#include "TB6612.h"
#include "delay.h"
#include "OLED.h"
#include "Usart5.h"
// ����PID���� - λ�û����⻷��
float Position_KP = 0.7f, Position_KI = 0.0010f, Position_KD = 0.1f;

// ����PID���� - �ٶȻ����ڻ��� 
float Incremental_KP = 2.5f, Incremental_KI = 0.0f, Incremental_KD = 0.6f;

// �������״̬
static Motor_Control_State_t motor_states[4];

// PID����ʹ�ܱ�־
static uint8_t pid_control_enabled = 0;

/**************************************************************************
���ܣ���ʼ��PID����ģ��
��ڲ�������
����  ֵ����
˵������ʼ��TIM6Ϊ1kHz�жϣ�����PID����
**************************************************************************/
void PID_Control_Init(void)
{
    TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStrue;
    NVIC_InitTypeDef NVIC_InitStructure;
    
    // ʹ��TIM6ʱ��
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM6, ENABLE);
    
    // ����TIM6Ϊ1kHz�ж�Ƶ�� (36MHz/(35+1)/(999+1) = 1kHz)
    TIM_TimeBaseInitStrue.TIM_Period = 999;          // �Զ���װֵ999������1000��������
    TIM_TimeBaseInitStrue.TIM_Prescaler = 35;        // Ԥ��Ƶ35��TIM6��APB1ʱ��(36MHz)
    TIM_TimeBaseInitStrue.TIM_CounterMode = TIM_CounterMode_Up;
    TIM_TimeBaseInitStrue.TIM_ClockDivision = TIM_CKD_DIV1;
    TIM_TimeBaseInit(TIM6, &TIM_TimeBaseInitStrue);

    // ����TIM6�ж�
    TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);
    
    // ����NVIC�ж����ȼ�
    NVIC_InitStructure.NVIC_IRQChannel = TIM6_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1; // �����ȼ���ȷ��PID���㼰ʱִ��
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
    
    // ʹ�ܶ�ʱ��
    TIM_Cmd(TIM6, ENABLE);
}

/**************************************************************************
���ܣ�TIM6�жϷ������ - PID���Ƽ���
��ڲ�������
����  ֵ����
˵����ÿ1kHz����һ�Σ�ִ����·PID���Ƽ���ͱ�����λ���ۻ�
**************************************************************************/
void TIM6_IRQHandler(void)
{
    if (TIM_GetITStatus(TIM6, TIM_IT_Update) != RESET) {
        // ����жϱ�־
        TIM_ClearITPendingBit(TIM6, TIM_IT_Update);
        
        // ����PID���ƴ���
        if (pid_control_enabled) {
            // ��·��������PID���Ƽ���
            for (int i = 0; i < 4; i++) {
                if (motor_states[i].control_enabled) {
                    static int Moto[4] = {0};
                    
                    // ��ȡʵ���ٶ�
                    int Reality_Velocity = (int)Get_Motor_Speed((MotorNum_t)i);
                    // ��ȡ������ʵ��λ��
                    int Reality_Position = (int)Get_Accumulated_Position((MotorNum_t)i); 
                    
                    // λ��ʽPID���ƣ��⻷��- ���Ŀ���ٶ�
                    Moto[i] = Position_PID(Reality_Position, (int)motor_states[i].target_position, i);
                    
                    // λ�û�����޷���Ŀ���ٶȷ�Χ
                    Moto[i] = Xianfu(Moto[i], (int)motor_states[i].target_speed);
                    
                    // ����Ƿ񵽴�Ŀ��λ�ã�������̶ȼ�飩
                    if ((Reality_Position - (int)motor_states[i].target_position) < 1 && (Reality_Position - (int)motor_states[i].target_position) > -1) {
                        Incremental_PID(0, 0, i);  // ��������ʽPID�ۻ�״̬
                        Set_Motor_PWM((MotorNum_t)i, 0);
                        motor_states[i].finish_flag = 1;
                    } else {
                        // ����ʽPID���ƣ��ڻ���- �ٶȻ�����
                        Moto[i] = Incremental_PID(Reality_Velocity, Moto[i], i);
                        
                        // ����PWM���
                        Set_Motor_PWM((MotorNum_t)i, Moto[i]);
                    }
                }
            }
        }
    }
}

/**************************************************************************
���ܣ���ȡ�ۻ�λ�ò������ٶ�
��ڲ�����������
����  ֵ���ۻ�λ��ֵ
˵����ʵ�ֱ����������ۻ�������16λ��������������
**************************************************************************/
int32_t Get_Accumulated_Position(MotorNum_t motor)
{
    Motor_Control_State_t* state = &motor_states[motor];
    
    // ��ȡ��ǰ����������ֵ
    int16_t current_count = Get_Encoder_Count(motor);
    
    // ����˲ʱ�ٶȣ���ǰCNTֵ����1ms�ڵ���������ת��Ϊ����/��
    state->current_speed = (float)current_count * SPEED_CALC_FREQUENCY_HZ;
    
    // ����ǰCNTֱֵ���ۼӵ��ۻ�λ��
    state->accumulated_position += current_count;
    
    // ��ȡ���������������������
    Reset_Encoder(motor);
    
    return state->accumulated_position;
}

/**************************************************************************
���ܣ���ȡ���ԭʼ�ٶ�
��ڲ�����������
����  ֵ��ԭʼ�ٶ�ֵ������������/�룩
˵��������δ�˲���˲ʱ�ٶȣ���Ӧ��
**************************************************************************/
float Get_Motor_Speed(MotorNum_t motor)
{
    if (motor >= 4) return 0.0f; // �������
    
    return motor_states[motor].current_speed;
}


/**************************************************************************
���ܣ������ۻ�λ��
��ڲ�����������
����  ֵ����
˵��������ָ��������ۻ�λ��
**************************************************************************/
void Reset_Accumulated_Position(MotorNum_t motor)
{
    Motor_Control_State_t* state = &motor_states[motor];
    
    // ���ñ�����Ӳ��������
    Reset_Encoder(motor);
    
    // �����ۻ�״̬
    state->accumulated_position = 0;
    state->current_speed = 0.0f;
}

/**************************************************************************
���ܣ�����PID��������ʼ��
��ڲ�������
����  ֵ����
˵������ʼ������PID����ģ��
**************************************************************************/
void Motor_Cascade_PID_Init(void)
{
    int i;
    
    // ��ʼ��PID����ģ�飨TIM6��
    PID_Control_Init();
    
    // ��ʼ����·���״̬
    for (i = 0; i < 4; i++) {

        // ����Ĭ���ٶ�����
        motor_states[i].target_speed = 0.0f;  // ���ú����Ĭ���ٶ�����
        
        // �����ۻ�λ�ú��ٶ�״̬
        Reset_Accumulated_Position((MotorNum_t)i);
    }
    
    // ֹͣ���е��
    Move_stop();
    
    // ����PID����
    pid_control_enabled = 0;
}

/**************************************************************************
���ܣ����õ������Ŀ��λ��
��ڲ����������ţ�ת��Ȧ����������ת��������ת��
����  ֵ����
˵��������ָ�������Ŀ��λ�ã�������������
**************************************************************************/
void Motor_Set_Target(MotorNum_t motor, float revolutions)
{
    if (motor >= 4) return; // �������
    
    // ����Ŀ�����������ֵ
    motor_states[motor].target_position = revolutions * (ENCODER_COUNTS_PER_REVOLUTION-7.45);
    motor_states[motor].control_enabled = 1; // ʹ�ܸõ������
    
    // ���ö�Ӧ�ۻ�λ��
    Reset_Accumulated_Position(motor);
    
}

/**************************************************************************
���ܣ����õ������Ŀ��λ�ú��ٶ�����
��ڲ����������ţ�ת��Ȧ��������ٶ�����
����  ֵ����
˵������������ר�ýӿڣ�֧���ٶ����Ƶ�λ�ÿ���
**************************************************************************/
void Motor_Set_Target_With_Speed(MotorNum_t motor, float revolutions, float max_speed)
{
    if (motor >= 4) return; // �������
    
    // ����Ŀ��λ��
    Motor_Set_Target(motor, revolutions);
    
    // �����ٶ�����
    if(max_speed > 0) {
        motor_states[motor].target_speed = max_speed * (ENCODER_COUNTS_PER_REVOLUTION-7.45);
    } else {
        motor_states[motor].target_speed = 0.0f; 
    }

    Motor_Start_Control();
}

/**************************************************************************
���ܣ�����PID����
��ڲ�������
����  ֵ����
˵���������ж���������·����PID����
**************************************************************************/
void Motor_Start_Control(void)
{
    // ����PID����
    pid_control_enabled = 1;
}

/**************************************************************************
���ܣ�ֹͣPID����
��ڲ�������
����  ֵ����
˵����ֹͣ����PID���Ʋ�ֹͣ���е��
**************************************************************************/
void Motor_Stop_Control(void)
{
    int i;
    
    // ����PID����
    pid_control_enabled = 0;
    
    // ֹͣ���е��
    Move_stop();
    
    // �������е������
    for (i = 0; i < 4; i++) {
        motor_states[i].control_enabled = 0;
    }
}



/**************************************************************************
���ܣ�ֹͣ���е������������״̬
��ڲ�������
����  ֵ����
˵����ֹͣ���е��
**************************************************************************/
void Stop_All_Motor(void)
{
    Motor_Set_Target_With_Speed(Motor1, 0, 100);    
    Motor_Set_Target_With_Speed(Motor2, 0, 100);    
    Motor_Set_Target_With_Speed(Motor3, 0, 100);    
    Motor_Set_Target_With_Speed(Motor4, 0, 100);    
}

/**************************************************************************
���ܣ�λ��ʽPID������
��ڲ�����ʵ��λ�ã�Ŀ��λ�ã�������
����  ֵ�����PWM��Ŀ���ٶȣ�
˵����λ��ʽ��ɢPID��ʽ 
pwm=Kp*e(k)+Ki*��e(k)+Kd[e(k)-e(k-1)]
e(k)����ǰ���ƫ�� 
e(k-1)����һ�ε�ƫ��  
��e(k)�����e(k)�Լ�֮ǰ��ƫ����ۻ���;����Ϊk=1,2,...,k;
pwm��������
**************************************************************************/
int Position_PID(int reality, int target, int motor_index)
{ 	
    static float Bias[4], Pwm[4], Last_Bias[4], Integral_bias[4] = {0};
    
    if(motor_index >= 4) return 0; // �������
    
    Bias[motor_index] = target - reality;                    // ����ƫ��
    Integral_bias[motor_index] += Bias[motor_index];	     // ƫ���ۻ�
    
    if(Integral_bias[motor_index] > 5000) Integral_bias[motor_index] = 5000;   // �����޷�
    if(Integral_bias[motor_index] < -5000) Integral_bias[motor_index] = -5000;
    
    Pwm[motor_index] = (Position_KP * Bias[motor_index])                        // ���������
                     + (Position_KI * Integral_bias[motor_index])               // ���������
                     + (Position_KD * (Bias[motor_index] - Last_Bias[motor_index])); // ΢�������
    
    Last_Bias[motor_index] = Bias[motor_index];                                 // �����ϴ�ƫ��
    
    return (int)Pwm[motor_index];                                               // �������
}

/**************************************************************************
���ܣ�����ʽPID������
��ڲ�����ʵ��ֵ��Ŀ��ֵ��������
����  ֵ�����PWM
˵��������ʽ��ɢPID��ʽ 
pwm+=Kp[e(k)-e(k-1)]+Ki*e(k)+Kd[e(k)-2e(k-1)+e(k-2)]
e(k)����ǰ���ƫ�� 
e(k-1)����һ�ε�ƫ��  �˴������ٶȿ��� 
pwm������������
**************************************************************************/
int Incremental_PID(int reality, int target, int motor_index)
{ 	
    static float Bias[4], Pwm[4] = {0}, Last_bias[4] = {0}, Prev_bias[4] = {0};
    
    if(motor_index >= 4) return 0; // �������
    
    Bias[motor_index] = target - reality;                                       // ����ƫ��
    
    Pwm[motor_index] += (Incremental_KP * (Bias[motor_index] - Last_bias[motor_index]))        // ���������
                      + (Incremental_KI * Bias[motor_index])                                   // ���������
                      + (Incremental_KD * (Bias[motor_index] - 2*Last_bias[motor_index] + Prev_bias[motor_index])); // ΢�������
    
    Prev_bias[motor_index] = Last_bias[motor_index];                            // �������ϴ�ƫ��
    Last_bias[motor_index] = Bias[motor_index];	                                // ������һ��ƫ��
    
    return (int)Pwm[motor_index];                                               // �������
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