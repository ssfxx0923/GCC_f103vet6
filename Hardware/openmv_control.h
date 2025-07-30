#ifndef __OPENMV_CONTROL_H
#define __OPENMV_CONTROL_H

#include "sys.h"

// ��Э�鳣������ - 5�ֽ�Э�飺[0xAA, ģʽλ, PIDλ, �����߱�־λ, ��ɫλ]
#define PACKET_SYNC 0xAA

// ģʽλ����
#define MODE_FORWARD  0x00   // ��ǰѲ��ģʽ
#define MODE_BACKWARD 0x01   // ���Ѳ��ģʽ
#define MODE_COLOR    0x02   // ��ɫʶ��ģʽ

// �����߱�־λ����
#define CROSS_NONE     0x00  // �޽�����
#define CROSS_DETECTED 0x01  // ��⵽������

// ��ɫʶ�����
typedef enum {
    COLOR_NONE = 0,
    COLOR_RED,
    COLOR_BLACK,
    COLOR_WHITE,  
    COLOR_BLUE,
    COLOR_GREEN
} ColorType_t;

// ��������
void OpenMV_Control_Init(void);
void OpenMV_Process_Data(uint8_t *data, uint8_t len);
void OpenMV_Line_Following_Control(float speed);
void OpenMV_Go_Control(float speed,uint32_t count,float revolutions);
void OpenMV_Send_Command(uint8_t mode);
uint32_t OpenMV_Get_Cross_Count(void);
int8_t OpenMV_Get_PID_Output(void);
ColorType_t OpenMV_Get_Color(void);

#endif 