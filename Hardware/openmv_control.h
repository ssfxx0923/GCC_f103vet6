#ifndef __OPENMV_CONTROL_H
#define __OPENMV_CONTROL_H

#include "sys.h"

// Э�鳣������ 
#define PACKET_SYNC 0xAA

// ģʽλ����
#define MODE_FORWARD  0x00   // ��ǰѲ��ģʽ
#define MODE_COLOR    0x01   // ��ɫʶ��ģʽ
#define MODE_TURN_ASSIST 0x02 // ת�丨��ģʽ

// ��־λ����
#define FLAG_NONE     0x00  // �ޱ�־
#define CROSS_DETECTED 0x01  // ��⵽������
#define TURN_DETECTED 0x02  // ��⵽ת��
// ��ɫʶ����� - ��Ϊ���ֱ��
#define COLOR_COUNT 5  // ����¼5����ɫ

// ͨ�������
#define CMD_COLOR_RECORDED 0x20  // ��ɫ��¼ȷ������

// ��������
void OpenMV_Control_Init(void);
void OpenMV_Process_Data(uint8_t *data, uint8_t len);
void OpenMV_Line_Forward_Control(float speed);
void OpenMV_Go_Control(float speed,uint32_t count,float revolutions);
void OpenMV_Turn_Control(float direction, uint32_t target_lines, float speed);
void OpenMV_Send_Command(uint8_t mode);
void OpenMV_Send_Color_Recorded(uint8_t color_index);
uint32_t OpenMV_Get_Cross_Count(void);
uint32_t OpenMV_Get_Turn_Count(void);
int8_t OpenMV_Get_PID_Output(void);
uint8_t OpenMV_Get_Color(uint8_t index);  // ͨ������ѡ�񷵻ض�Ӧ��ŵ���ɫ
void OpenMV_Record_Color(uint8_t color_value);  // ��¼��ɫ��ָ��λ��
uint8_t* OpenMV_Get_Color_Array(void);  // ��ȡ��ɫ����ָ��

#endif 
