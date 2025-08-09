#ifndef __OPENMV_CONTROL_H
#define __OPENMV_CONTROL_H

#include "sys.h"

// 协议常量
#define PACKET_SYNC 0xAA
#define COLOR_COUNT 5

// 模式定义 [FORWARD, COLOR, TURN_ASSIST]
#define MODE_FORWARD     0x00
#define MODE_COLOR       0x01
#define MODE_TURN_ASSIST 0x02

// 标志定义 [NONE, CROSS, TURN]
#define FLAG_NONE        0x00
#define CROSS_DETECTED   0x01
#define TURN_DETECTED    0x02

// 命令定义 [RECORDED, DETECT, RESULT]
#define CMD_COLOR_RECORDED 0x20
#define CMD_COLOR_DETECT   0x21
#define CMD_COLOR_RESULT   0x22

// 函数声明
void OpenMV_Control_Init(void);
void OpenMV_Process_Data(uint8_t *data, uint8_t len);
void OpenMV_Line_Forward_Control(float speed);
void OpenMV_Go_Control(float speed, uint32_t count, float revolutions);
void OpenMV_Turn_Control(float direction, uint32_t target_lines, float speed);
void OpenMV_Send_Command(uint8_t mode);
void OpenMV_Send_Color_Recorded(uint8_t color_index);

// Getter函数
uint32_t OpenMV_Get_Cross_Count(void);
uint32_t OpenMV_Get_Turn_Count(void);
int8_t OpenMV_Get_PID_Output(void);
uint8_t OpenMV_Get_Color(uint8_t index);

// 颜色检测函数
uint8_t OpenMV_Request_Color_Detection(uint8_t index);
void OpenMV_Record_Color(uint8_t color_value);

#endif 
