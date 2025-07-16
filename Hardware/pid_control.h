#ifndef __PID_CONTROL_H
#define __PID_CONTROL_H

#include "sys.h"
#include "encoder.h"

// PID������غ궨��
#define SPEED_CALC_FREQUENCY_HZ 1000     // �ٶȼ���Ƶ��1kHz

// �������״̬�ṹ��
typedef struct {
    float target_position;          // Ŀ��λ��(����������)
    float target_speed;             // Ŀ���ٶ�(����������/��)
    int32_t accumulated_position;   // �ۻ�λ��
    float current_speed;            // ��ǰ�ٶ�(����������/��)
    uint8_t finish_flag;            //�˶���ɱ�־
    uint8_t control_enabled;        // ����ʹ�ܱ�־
} Motor_Control_State_t;

// PID������غ�������
void PID_Control_Init(void);                                       // ��ʼ��PID����ģ��

// ����PID���ƺ�������
void Motor_Cascade_PID_Init(void);                           // ��ʼ������PID������
void Motor_Set_Target(MotorNum_t motor, float revolutions);   // ���õ����Ŀ��λ��
void Motor_Start_Control(void);                               // ����PID����
void Motor_Stop_Control(void);                                // ֹͣPID����
void Motor_Set_Target_With_Speed(MotorNum_t motor, float revolutions, float max_speed); // ����λ�ú��ٶ�����
void TurnGo(float circle, float speed);
void Stop_All_Motor(void);
// ������λ���ۻ���������
int32_t Get_Accumulated_Position(MotorNum_t motor);                // ��ȡ�ۻ�λ��
void Reset_Accumulated_Position(MotorNum_t motor);                 // �����ۻ�λ��
float Get_Motor_Speed(MotorNum_t motor);                          // ��ȡԭʼ�ٶ�


int Position_PID(int reality, int target, int motor_index);    // λ�û�PID
int Incremental_PID(int reality, int target, int motor_index); // �ٶȻ�PID
#endif 
