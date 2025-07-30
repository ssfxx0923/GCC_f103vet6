#ifndef __PID_CONTROL_H
#define __PID_CONTROL_H

#include "sys.h"
#include "encoder.h"

#define CONTROL_FREQUENCY_HZ        1000        // PID����Ƶ��1kHz
#define POSITION_TOLERANCE          2           // λ�ÿ���������̶�
#define POSITION_INTEGRAL_LIMIT     5000.0f     // λ�û������޷�
#define SPEED_OUTPUT_LIMIT          100.0f       // �ٶȻ�����޷�
#define SPEED_INTEGRAL_LIMIT        100.0f      // �ٶȻ������޷�

typedef enum {
    CONTROL_SPEED = 0,      // �ٶȿ���ģʽ
    CONTROL_POSITION        // λ�ÿ���ģʽ
} Motor_Control_Mode_t;

// �������״̬�ṹ��
typedef struct {
    Motor_Control_Mode_t control_mode;  // ����ģʽ 
    float target_position;              // Ŀ��λ��(����������)
    float target_speed;                 // Ŀ���ٶ�(RPM)
    float max_speed;                    // ��������ٶ�(RPM)
    int32_t accumulated_position;       // �ۻ�λ��
    float current_speed;                // ��ǰ�ٶ�(RPM)
} Motor_Control_State_t;

void PID_Control_Init(void);                                           // PID����ģ���ʼ��
void Position_Control_Start(MotorNum_t motor, float revolutions, float max_speed_rpm);  // ����λ�ÿ���
void Speed_Control_Start(MotorNum_t motor, float rpm);                 // �����ٶȿ���
void Motor_Control_Stop(MotorNum_t motor);                             // ֹͣ�����������
void Motor_Control_Stop_All(void);                                     // ֹͣ���е������
void Reset_Motor_State(MotorNum_t motor);                              // ���õ��״̬
int32_t Get_Accumulated_Position(MotorNum_t motor);                     // ��ȡ�ۻ�λ��
void Reset_Accumulated_Position(MotorNum_t motor);                      // �����ۻ�λ��
float Get_Motor_Speed(MotorNum_t motor);                               // ��ȡ����ٶ�(RPM)
float Position_PID_RPM(int current_position, int target_position, int motor_index);         // λ�û�PID(���RPM)
float Speed_PID_Calculate(float current_speed_rpm, float target_speed_rpm, int motor_index,
                         float KP, float KI, float KD);                                    // �ٶȻ�PID(RPM���룬�����ò���)
void Reset_Speed_PID(MotorNum_t motor);                                                     // �����ٶ�PID״̬
void Position_Control_Start_All(float revolutions, float max_speed_rpm);
float Limit_Value(float value, float limit);     

#endif 
