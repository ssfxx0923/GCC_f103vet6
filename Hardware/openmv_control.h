#ifndef __OPENMV_CONTROL_H
#define __OPENMV_CONTROL_H

#include "sys.h"

// 协议常量定义 
#define PACKET_SYNC 0xAA

// 模式位定义
#define MODE_FORWARD  0x00   // 向前巡线模式
#define MODE_COLOR    0x01   // 颜色识别模式
#define MODE_TURN_ASSIST 0x02 // 转弯辅助模式

// 标志位定义
#define FLAG_NONE     0x00  // 无标志
#define CROSS_DETECTED 0x01  // 检测到交叉线
#define TURN_DETECTED 0x02  // 检测到转弯
// 颜色识别相关
typedef enum {
    COLOR_NONE = 0,
    COLOR_RED,
    COLOR_BLACK,
    COLOR_WHITE,  
    COLOR_BLUE,
    COLOR_GREEN
} ColorType_t;

// 函数声明
void OpenMV_Control_Init(void);
void OpenMV_Process_Data(uint8_t *data, uint8_t len);
void OpenMV_Line_Forward_Control(float speed);
void OpenMV_Go_Control(float speed,uint32_t count,float revolutions);
void OpenMV_Turn_Control(float direction, uint32_t target_lines, float speed);
void OpenMV_Send_Command(uint8_t mode);
uint32_t OpenMV_Get_Cross_Count(void);
uint32_t OpenMV_Get_Turn_Count(void);
int8_t OpenMV_Get_PID_Output(void);
ColorType_t OpenMV_Get_Color(void);

#endif 
