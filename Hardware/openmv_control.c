#include "openmv_control.h"
#include "Usart2.h"
#include "TB6612.h"
#include "oled.h"
static int8_t pid_output = 0;
static uint8_t data_ready = 0;
static uint32_t cross_count = 0;
static uint8_t current_mode = MODE_FORWARD;  // ��ǰOpenMVģʽ
static uint8_t cross_status = CROSS_NONE;   // ��ǰ������״̬
static ColorType_t detected_color = COLOR_NONE;  // ��⵽����ɫ

void OpenMV_Control_Init(void)
{
    pid_output = 0;
    data_ready = 0;
    cross_count = 0;
    current_mode = MODE_FORWARD;  // Ĭ����ǰѲ��
    cross_status = CROSS_NONE;
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
        cross_status = data[3];
        if(cross_status == CROSS_DETECTED) {
            cross_count++;  // �ۼƽ����߼�����
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
        
        data_ready = 1;
    }
}


void OpenMV_Line_Following_Control(float speed)
{
    if(!data_ready) return;
    
    float left_speed, right_speed;
    
    // ����OpenMV���ݵ�ģʽ��Ϣ�����Ƶ��
    if(current_mode == MODE_BACKWARD) {
        // ���Ѳ��ģʽ���ٶ�Ϊ��ֵ��PID���Ʒ�����ͬ
        left_speed = -speed - pid_output * 0.3f;
        right_speed = -speed + pid_output * 0.3f;
    } else if(current_mode == MODE_FORWARD) {
        // ��ǰѲ��ģʽ��ԭ���߼�
        left_speed = speed + pid_output * 0.3f;
        right_speed = speed - pid_output * 0.3f;
    } else {
        // ��ɫʶ��ģʽ��ֹͣ�˶�
        left_speed = right_speed = 0.0f;
    }
    
    Speed_Control_Start(Motor1, left_speed);
    Speed_Control_Start(Motor2, right_speed);
    Speed_Control_Start(Motor3, left_speed);
    Speed_Control_Start(Motor4, right_speed);
    
    data_ready = 0;
}


void OpenMV_Go_Control(float speed,uint32_t count,float revolutions)
{
    while(1)
    {
        OpenMV_Line_Following_Control(speed);

        OLED_ShowNum(1,1,OpenMV_Get_PID_Output(),3);
        OLED_ShowNum(2,1,OpenMV_Get_Cross_Count(),3);
        if(cross_count == count)
        {   
            OLED_ShowNum(2,1,OpenMV_Get_Cross_Count(),3);
            Position_Control_Start_All(revolutions, speed);
            cross_count = 0;
            break;
        }
    }
}




void OpenMV_Send_Command(uint8_t mode)
{
    if(mode == 0) {
        Usart2_SendString("FORWARD\n");
    } else if(mode == 1) {
        Usart2_SendString("BACK\n");
    } else if(mode == 2) {
        Usart2_SendString("COLOR\n");
    }
}


// Getter����
uint32_t OpenMV_Get_Cross_Count(void)
{
    return cross_count;
}

int8_t OpenMV_Get_PID_Output(void)
{
    return pid_output;
}

ColorType_t OpenMV_Get_Color(void)
{
    return detected_color;
}

