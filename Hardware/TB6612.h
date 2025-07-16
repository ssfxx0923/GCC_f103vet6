#ifndef __TB6612_H 
#define __TB6612_H 
#include "sys.h"
#include "encoder.h"
#include "pid_control.h"

// PWM��غ궨��
#define PWM_MAX_VALUE 99     // PWM���ֵ (0-99)��ƥ��TIM8��ARR����

// ���������������
void MOTOR_Init(void);                                      // �����ʼ��
void Hardware_PWM_Init(void);                               // Ӳ��PWM��ʼ��
void Motor_Direction_Init(void);                            // ����������ų�ʼ��
void Move_stop(void);                                       // ֹͣ���е��

// ���Ӳ�����ƺ�������
void Motor_SetDirection(MotorNum_t motor, int direction);   // ���õ������
void Motor_SetPWM(MotorNum_t motor, int pwm);              // ���õ��PWMֵ

// ����PID������ص����������
void Set_Motor_PWM(MotorNum_t motor, int pwm);            // ����PWM�ͷ��򣨺�����������(-100��100)
int Xianfu(int data, int max);                             // �޷�����

#endif
