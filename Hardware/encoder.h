#ifndef __ENCODER_H
#define __ENCODER_H

#include "sys.h"

// ��������غ궨��
#define ENCODER_COUNTS_PER_REVOLUTION 1560  // ���תһȦ����������ֵ

// �����Ŷ���
typedef enum {
    Motor1 = 0,  // ��ӦTIM1������
    Motor2 = 1,  // ��ӦTIM2������
    Motor3 = 2,  // ��ӦTIM3������
    Motor4 = 3   // ��ӦTIM4������
} MotorNum_t;


// ���������������
void Encoder_Init(void);                          // ��ʼ�����б�����
int16_t Get_Encoder_Count(MotorNum_t motor);      // ��ȡָ������������
void Reset_Encoder(MotorNum_t motor);             // ����ָ������������
void Reset_All_Encoders(void);                    // �������б���������

#endif
