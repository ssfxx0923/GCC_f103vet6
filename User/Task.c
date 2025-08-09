#include "Task.h"
#include "openmv_control.h"
#include "oled.h"
#include "delay.h"
#include "MoveSet.h"
#include "ServoMoveSet.h"

static int8_t current_direction = 0;
static int8_t current_position = 0;
static int8_t current_process = 0;
static int8_t sleeve_flag = 0;
static int8_t claw_flag = 0;
static int8_t map_flag [5] = {1,0,1,0,1};
static int8_t end_flag [5] = {0,0,0,0,0};




void go_back_center();

void task1_run(){

    OpenMV_Go_Control(40,3,0); //ֱ��ǰ��C��
    current_direction = 3;
    delay_ms(1000);
    OpenMV_Request_Color_Detection(current_process);
    //ץȡ���
    sleeve_flag = 1;
    map_flag[current_direction] = 0;
    if(current_direction == OpenMV_Get_Color(current_process)){ //��������ɫ��ȷ����ֱ��ǰ���������
        OpenMV_Go_Control(20,10,0);
        sleeve_flag = 0;
        end_flag[current_position] = 1;
    }
    else{
        OpenMV_Go_Control(20,10,0); //��������ɫ����ȷ���򷵻�����
        current_position = 0;
    }
    for(int loop = 0; loop < 2; loop++){
        if(current_position != 0){go_back_center();current_position = 0;}
        if(sleeve_flag||claw_flag){ //�����ǰ��Ͳ����צ�������
            OpenMV_Turn_Control(OpenMV_Get_Color(current_process)-current_direction,20); // ת������Ӧ��ɫ�ķ���
            current_direction = OpenMV_Get_Color(current_process);
            if(!sleeve_flag&&claw_flag){ // �����ǰ��צ�ӣ����佻����Ͳ
                OpenMV_Go_Control(20,10,0);
                sleeve_flag = 1;
                claw_flag = 0;
            }
            if(map_flag[current_direction]){ //�����ͼ������飬����צ�ӹ�ȡ
                
                OpenMV_Request_Color_Detection(++current_process);
                OpenMV_Go_Control(20,10,0);
                map_flag[current_direction] = 0;
                claw_flag = 1;
                current_position = 1;

                OpenMV_Go_Control(20,10,0); //ǰ���������
                current_position = 2;
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }
            else{
                OpenMV_Go_Control(20,10,0); //�����ͼ��û����飬��ֱ��ǰ���������
                current_position = 2;
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }

        }
        else{   //�����ǰ��Ͳצ�Ӷ�û�����
            for(int i = 0; i < 5; i++){ //������ͼ���ҵ����
                if(map_flag[i]){
                    OpenMV_Turn_Control(++i-current_direction,20); //ת���������ķ���
                    current_direction = i;
                    break;
                }
            }

            OpenMV_Go_Control(20,10,0); //ǰ�����
            OpenMV_Request_Color_Detection(++current_process);
            current_position = 1;
            sleeve_flag = 1;
            map_flag[current_direction] = 0;
            
            if(current_direction == OpenMV_Get_Color(current_process)){ //��������ɫ��ȷ����ֱ��ǰ���������
                OpenMV_Go_Control(20,10,0);
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }
            else{ //��������ɫ����ȷ���򷵻�����
                go_back_center();
                current_position = 0;
            }


        }
    }
    
}

// void task2_run(){
//     if(current_position != 0){go_back_center();current_position = 0;}
//     OpenMV_Turn_Control(-1-current_direction,20);
//     OpenMV_Go_Control(20,10,0);     //���ϵ�1
//     OpenMV_Request_Color_Detection(++current_process)
