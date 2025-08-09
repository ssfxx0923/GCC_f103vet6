#include "openmv_control.h"
#include "Usart2.h"
#include "TB6612.h"
#include "oled.h"
#include "delay.h"

// 状态变量
static int8_t pid_output = 0;
static uint32_t cross_count = 0;
static uint32_t turn_count = 0;
static uint8_t current_mode = MODE_FORWARD;
static uint8_t flag_status = FLAG_NONE;
static uint8_t detected_color = 0;
static uint8_t color_array[COLOR_COUNT];
static uint8_t color_count = 0;
static volatile uint8_t color_detect_pending = 0xFF;  // 0xFF表示无等待
static volatile uint8_t color_detect_result = 0;
static volatile uint8_t color_detect_complete = 0;

void OpenMV_Control_Init(void)
{
    pid_output = cross_count = turn_count = 0;
    current_mode = MODE_FORWARD;
    flag_status = FLAG_NONE;
    detected_color = color_count = 0;
    
    for(int i = 0; i < COLOR_COUNT; i++) {
        color_array[i] = 0;
    }
    
    color_detect_pending = 0xFF;
    color_detect_result = color_detect_complete = 0;
}

void OpenMV_Process_Data(uint8_t *data, uint8_t len)
{
    // 打印接收到的数据包
    // printf("接收数据包: 长度=%d, 数据=[", len);
    // for(int i = 0; i < len; i++) {
    //     printf("0x%02X", data[i]);
    //     if(i < len-1) printf(", ");
    // }
    // printf("]\r\n");
    
    // 处理颜色检测结果
    if(len >= 3 && data[0] == CMD_COLOR_RESULT) {
        uint8_t index = data[1], color = data[2];
        
        // printf("收到颜色结果: 索引=%d, 颜色=%d\r\n", index, color);
        
        if(color_detect_pending == index) {
            color_detect_result = color;
            color_detect_complete = 1;
            
            if(index < COLOR_COUNT) {
                color_array[index] = color;
                color_count = (index >= color_count) ? index + 1 : color_count;
                // printf("记录颜色: 索引=%d, 颜色=%d\r\n", index, color);
            }
            
            OpenMV_Send_Color_Recorded(index);
        }
        return;
    }
    
    // 处理主要数据包 [同步位, 模式, PID, 标志, 颜色]
    if(len >= 5 && data[0] == PACKET_SYNC) {
        current_mode = data[1];
        
        // PID字节转有符号整数
        pid_output = (data[2] > 127) ? (int8_t)(data[2] - 256) : (int8_t)data[2];
        
        // 处理标志位
        flag_status = data[3];
        if(flag_status == CROSS_DETECTED) cross_count++;
        if(flag_status == TURN_DETECTED) turn_count++;
        
        detected_color = data[4];
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
    left_speed = speed + pid_output * 0.9f;
    right_speed = speed - pid_output * 0.9f;
    
    Speed_Control_Start(Motor1, left_speed);
    Speed_Control_Start(Motor2, right_speed);
    Speed_Control_Start(Motor3, left_speed);
    Speed_Control_Start(Motor4, right_speed);
}

void OpenMV_Go_Control(float speed,uint32_t count,float revolutions)
{
    uint32_t timeout_counter = 0;
    const uint32_t max_timeout = 50000;  
    cross_count = 0;
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
    OLED_ShowNum(3,1,cross_count,3);
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
    turn_count = 0; 
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
    delay_ms(100);
    for(int i = 0; i < 20; i++)
    {
        OpenMV_Line_Forward_Control(0);
    }
    Position_Control_Start_All(0,speed);
}


void OpenMV_Send_Command(uint8_t mode)
{
    Usart2_SendChar(mode);
}

void OpenMV_Send_Color_Recorded(uint8_t color_index)
{
    uint8_t data[2] = {CMD_COLOR_RECORDED, color_index};
    for(int i = 0; i < 2; i++) {
        Usart2_SendChar(data[i]);
    }
}


// Getter函数
uint32_t OpenMV_Get_Cross_Count(void) { return cross_count; }
uint32_t OpenMV_Get_Turn_Count(void) { return turn_count; }
int8_t OpenMV_Get_PID_Output(void) { return pid_output; }

uint8_t OpenMV_Get_Color(uint8_t index)
{   
    return (index < COLOR_COUNT && index < color_count) ? color_array[index] : 0;
}

uint8_t OpenMV_Request_Color_Detection(uint8_t index)
{
    if(index >= COLOR_COUNT) return 0;
    
    // 发送检测请求
    uint8_t cmd_data[2] = {CMD_COLOR_DETECT, index};
    color_detect_pending = index;
    color_detect_result = color_detect_complete = 0;
    
    for(int i = 0; i < 2; i++) {
        Usart2_SendChar(cmd_data[i]);
    }
    
    // 等待结果（5秒超时）
    for(uint32_t timeout = 0; !color_detect_complete && timeout < 5000; timeout++) {
        delay_ms(1);
    }
    
    color_detect_pending = 0xFF;
    return color_detect_complete ? color_detect_result : 0;
}

void OpenMV_Record_Color(uint8_t color_value)
{
    // 检查是否已记录过此颜色
    for(int i = 0; i < color_count; i++) {
        if(color_array[i] == color_value) return;
    }
    
    // 添加新颜色（如果有空间）
    if(color_count < COLOR_COUNT) {
        color_array[color_count++] = color_value;
    }
}

