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
// 颜色识别相关 - 简化为数字标号
#define COLOR_COUNT 5  // 最多记录5种颜色

// 通信命令定义
#define CMD_COLOR_RECORDED 0x20  // 颜色记录确认命令

// 函数声明
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
uint8_t OpenMV_Get_Color(uint8_t index);  // 通过参数选择返回对应标号的颜色
void OpenMV_Record_Color(uint8_t color_value);  // 记录颜色到指定位置
uint8_t* OpenMV_Get_Color_Array(void);  // 获取颜色数组指针

#endif 
