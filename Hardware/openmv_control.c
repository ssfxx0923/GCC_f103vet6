#include "openmv_control.h"
#include "Usart2.h"
#include "TB6612.h"
#include "oled.h"
#include "delay.h"
static int8_t pid_output = 0;
static uint32_t cross_count = 0;
static uint32_t turn_count = 0;
static uint8_t current_mode = MODE_FORWARD;  // 当前OpenMV模式
static uint8_t flag_status = FLAG_NONE;   // 当前标志状态
static ColorType_t detected_color = COLOR_NONE;  // 检测到的颜色

void OpenMV_Control_Init(void)
{
    pid_output = 0;
    cross_count = 0;
    turn_count = 0;
    current_mode = MODE_FORWARD;  // 默认向前巡线
    flag_status = FLAG_NONE;
    detected_color = COLOR_NONE;  // 默认无颜色检测
}

void OpenMV_Process_Data(uint8_t *data, uint8_t len)
{
    // [0xAA, 模式位, PID位, 交叉线标志位, 颜色位]
    if(len >= 5 && data[0] == PACKET_SYNC) {
        // 解析模式位
        current_mode = data[1];
        
        // 解析PID位：将字节转换为有符号整数
        uint8_t pid_byte = data[2];
        if(pid_byte > 127) {
            // 负数：补码转换
            pid_output = (int8_t)(pid_byte - 256);
        } else {
            // 正数
            pid_output = (int8_t)pid_byte;
        }
        
        // 解析交叉线标志位
        flag_status = data[3];
        if(flag_status == CROSS_DETECTED) {
            cross_count++;  // 累计交叉线检测次数
        }

        if(flag_status == TURN_DETECTED) {
            turn_count++;  // 累计转弯检测次数
        }
        
        // 解析颜色位
        uint8_t color_byte = data[4];
        switch(color_byte) {
            case 1: detected_color = COLOR_RED; break;
            case 2: detected_color = COLOR_BLACK; break;
            case 3: detected_color = COLOR_WHITE; break;
            case 4: detected_color = COLOR_BLUE; break;
            case 5: detected_color = COLOR_GREEN; break;
            default: detected_color = COLOR_NONE; break;
        }
    }
}


void OpenMV_Line_Forward_Control(float speed)
{
    float left_speed, right_speed;
    if(current_mode != MODE_FORWARD)    
    {
        OpenMV_Send_Command(0);
        delay_ms(100);
    }
    left_speed = speed + pid_output * 0.7f;
    right_speed = speed - pid_output * 0.7f;
    
    Speed_Control_Start(Motor1, left_speed);
    Speed_Control_Start(Motor2, right_speed);
    Speed_Control_Start(Motor3, left_speed);
    Speed_Control_Start(Motor4, right_speed);
}

void OpenMV_Go_Control(float speed,uint32_t count,float revolutions)
{
    uint32_t timeout_counter = 0;
    const uint32_t max_timeout = 50000;  
    
    while(timeout_counter < max_timeout)
    {
        OpenMV_Line_Forward_Control(speed);
        timeout_counter++;
        OLED_ShowNum(2,1,cross_count,3);
        if(cross_count >= count)
        {   
            OLED_ShowNum(2,1,cross_count,3);
            break;
        }
    }
    Position_Control_Start_All(revolutions, speed);
    cross_count = 0;
}

void OpenMV_Turn_Control(float direction, uint32_t target_lines,float speed)
{
    if(current_mode != MODE_TURN_ASSIST)
    {
        OpenMV_Send_Command(2);  // 切换到转弯辅助模式
        delay_ms(100);
    }
    
    uint32_t timeout_counter = 0;
    const uint32_t max_timeout = 50000;  
    
    while(timeout_counter < max_timeout)
    {
        if(direction > 0)
        {
            Speed_Control_Start(Motor1, speed);
            Speed_Control_Start(Motor2, -speed);
            Speed_Control_Start(Motor3, speed);
            Speed_Control_Start(Motor4, -speed);
        }
        else
        {
            Speed_Control_Start(Motor1, -speed);
            Speed_Control_Start(Motor2, speed);
            Speed_Control_Start(Motor3, -speed);
            Speed_Control_Start(Motor4, speed);
        }
        timeout_counter++;
        OLED_ShowNum(1,1,turn_count,3);
        if(turn_count >= target_lines)
        {   
            OLED_ShowNum(1,1,turn_count,3);
            break;
        }
    }
    OpenMV_Send_Command(0);
    turn_count = 0;  
    delay_ms(100);
    for(int i = 0; i < 20; i++)
    {
        OpenMV_Line_Forward_Control(0);
    }
    Position_Control_Start_All(0,speed);
}


void OpenMV_Send_Command(uint8_t mode)
{
    // 发送单字节命令，与模式常量对应
    Usart2_SendChar(mode);
}


// Getter函数
uint32_t OpenMV_Get_Cross_Count(void)
{
    return cross_count;
}

uint32_t OpenMV_Get_Turn_Count(void)
{
    return turn_count;
}

int8_t OpenMV_Get_PID_Output(void)
{
    return pid_output;
}

ColorType_t OpenMV_Get_Color(void)
{   
    if(current_mode != MODE_COLOR)
    {
        OpenMV_Send_Command(1);
        delay_ms(100);
    }
    return detected_color;
}

