#include "openmv_control.h"
#include "Usart2.h"
#include "TB6612.h"
#include "oled.h"
#include "delay.h"
static int8_t pid_output = 0;
static uint32_t cross_count = 0;
static uint32_t turn_count = 0;
static uint8_t current_mode = MODE_FORWARD;  // ��ǰOpenMVģʽ
static uint8_t flag_status = FLAG_NONE;   // ��ǰ��־״̬
static ColorType_t detected_color = COLOR_NONE;  // ��⵽����ɫ

void OpenMV_Control_Init(void)
{
    pid_output = 0;
    cross_count = 0;
    turn_count = 0;
    current_mode = MODE_FORWARD;  // Ĭ����ǰѲ��
    flag_status = FLAG_NONE;
    detected_color = COLOR_NONE;  // Ĭ������ɫ���
}

void OpenMV_Process_Data(uint8_t *data, uint8_t len)
{
    // [0xAA, ģʽλ, PIDλ, �����߱�־λ, ��ɫλ]
    if(len >= 5 && data[0] == PACKET_SYNC) {
        // ����ģʽλ
        current_mode = data[1];
        
        // ����PIDλ�����ֽ�ת��Ϊ�з�������
        uint8_t pid_byte = data[2];
        if(pid_byte > 127) {
            // ����������ת��
            pid_output = (int8_t)(pid_byte - 256);
        } else {
            // ����
            pid_output = (int8_t)pid_byte;
        }
        
        // ���������߱�־λ
        flag_status = data[3];
        if(flag_status == CROSS_DETECTED) {
            cross_count++;  // �ۼƽ����߼�����
        }

        if(flag_status == TURN_DETECTED) {
            turn_count++;  // �ۼ�ת�������
        }
        
        // ������ɫλ
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
    OpenMV_Send_Command(0);

    left_speed = speed + pid_output * 0.3f;
    right_speed = speed - pid_output * 0.3f;
    
    Speed_Control_Start(Motor1, left_speed);
    Speed_Control_Start(Motor2, right_speed);
    Speed_Control_Start(Motor3, left_speed);
    Speed_Control_Start(Motor4, right_speed);
}


void OpenMV_Line_Backward_Control(float speed)
{
    float left_speed, right_speed;
    OpenMV_Send_Command(1);
    left_speed = -speed + pid_output * 0.3f;
    right_speed = -speed - pid_output * 0.3f;

    Speed_Control_Start(Motor1, left_speed);
    Speed_Control_Start(Motor2, right_speed);
    Speed_Control_Start(Motor3, left_speed);
    Speed_Control_Start(Motor4, right_speed);
}

void OpenMV_Go_Control(float speed,uint32_t count,float revolutions)
{
    while(1)
    {
        OLED_ShowNum(2,1,cross_count,3);
        OpenMV_Line_Forward_Control(speed);
        if(cross_count == count)
        {   
            OLED_ShowNum(2,1,cross_count,3);
            Position_Control_Start_All(revolutions, speed);
            cross_count = 0;
            break;
        }
    }
}


void OpenMV_Turn_Control(float direction, uint32_t target_lines,float speed)
{
    OpenMV_Send_Command(3);  // �л���ת�丨��ģʽ
    
    while(1)
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
        OLED_ShowNum(1,1,turn_count,3);
        if(turn_count >= target_lines)
        {   
            OLED_ShowNum(1,1,turn_count,3);
            OpenMV_Send_Command(0);  // �л�����ǰѲ��ģʽ
            turn_count = 0;  // ���ü���
            delay_ms(100);
            // ��׼��ǰ���ߣ�ʹ���෴�ٶȾ���
            for(int i = 0; i < 200; i++)
            {
                Speed_Control_Start(Motor1, -pid_output * 0.3f);
                Speed_Control_Start(Motor2, pid_output * 0.3f);
                Speed_Control_Start(Motor3, -pid_output * 0.3f);
                Speed_Control_Start(Motor4, pid_output * 0.3f);
                delay_ms(50);
            }
            Position_Control_Start_All(0,speed);
            break;
        }
    }
}


void OpenMV_Send_Command(uint8_t mode)
{
    // ���͵��ֽ������ģʽ������Ӧ
    Usart2_SendChar(mode);
}


// Getter����
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
    OpenMV_Send_Command(2);
    delay_ms(100);
    return detected_color;
}

