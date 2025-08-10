#include "Task.h"
#include "openmv_control.h"
#include "oled.h"
#include "delay.h"
#include "MoveSet.h"
#include "PCA9685.h"
static int8_t current_direction = 0;
static int8_t current_position = 0;
static int8_t current_block = 0;
static int8_t current_process = 0;
static int8_t sleeve_flag = 0;
static int8_t claw_flag = 0;
static int8_t map_flag [5] = {1,0,1,0,1};
static int8_t end_flag [5] = {0,0,0,0,0};




void go_back_center(){
    if(current_position == 0) return;
    else if(current_position == 1){Position_Control_Start_All(-1.45,40);delay_ms(1500);}
    else if(current_position == 2){
        Position_Control_Start_All(-1.2,40);
        delay_ms(1000);
        OpenMV_Turn_Control(2,20);
        current_direction = current_direction+4;
        if (current_direction>7){current_direction -= 8;}
        delay_ms(1000);
        OpenMV_Go_Control(50,1,1.65); // ǰ������
        delay_ms(1000);
    }
}




void task1_run(){

    OpenMV_Go_Control(50,3,0); //ֱ��ǰ��C��
    current_direction = 3;
    OpenMV_Request_Color_Detection(current_block);
    OLED_ShowNum(1,5,OpenMV_Get_Color(current_block),2);
    Position_Control_Start_All(0.7,40);
    delay_ms(1000);
    crazyMe(9,160,160,20,30);//ץȡ���
    current_position = 1;
    sleeve_flag = 1;
    map_flag[current_direction-1] = 0;
    if(current_direction == OpenMV_Get_Color(current_block)){ //��������ɫ��ȷ����ֱ��ǰ���������
        Position_Control_Start_All(0.3,40);
        OpenMV_Go_Control(50,1,0.62);
        current_position = 2;
        crazyMe(9,46,46,20,30);//�������
        sleeve_flag = 0;
        end_flag[current_position-1] = 1;
        current_process++;
    }
    while(current_process < 3){     //ÿ�η��ü�һ�������������������
        if(current_position != 0){go_back_center();current_position = 0;}
        if(sleeve_flag||claw_flag){ //�����ǰ��Ͳ����צ�������
            OpenMV_Turn_Control(OpenMV_Get_Color(current_block)-current_direction,20); // ת������Ӧ��ɫ�ķ���
            current_direction = OpenMV_Get_Color(current_block);
            if(!sleeve_flag&&claw_flag){ // �����ǰ��צ�ӣ����佻����Ͳ
                Position_Control_Start_All(0.5,40);
                sleeve_flag = 1;
                claw_flag = 0;
            }
            if(map_flag[current_direction-1]){ //�����ͼ������飬����צ�ӹ�ȡ
                OpenMV_Go_Control(50,1,0); //ǰ�����
                OpenMV_Request_Color_Detection(++current_block);
                Position_Control_Start_All(0.6,40);
                delay_ms(1000);
//                crazyMe(9,160,160,20,30);//ץȡ���
                map_flag[current_direction-1] = 0;
                claw_flag = 1;
                current_position = 1;
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }
            else{
                OpenMV_Go_Control(50,2,0.62); //�����ͼ��û����飬��ֱ��ǰ���������
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }

        }
        else{   //�����ǰ��Ͳצ�Ӷ�û�����
            OLED_ShowNum(4,4,current_direction,3);
            for(int i = 0; i < 5; i++){ //������ͼ���ҵ����
                if(map_flag[i]){
                    OpenMV_Turn_Control(++i-current_direction,20); //ת���������ķ���
                    current_direction = i;
                    break;
                }
            }
            OpenMV_Go_Control(50,1,0); //ǰ�����
            OpenMV_Request_Color_Detection(++current_block);
            Position_Control_Start_All(0.7,40);
            delay_ms(1000);
            crazyMe(9,160,160,20,30);//ץȡ���            
            current_position = 1;
            sleeve_flag = 1;
            map_flag[current_direction-1] = 0;            
            if(current_direction == OpenMV_Get_Color(current_block)){ //��������ɫ��ȷ����ֱ��ǰ���������
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }
            else{ //��������ɫ����ȷ���򷵻�����
                go_back_center();
                current_position = 0;
            }


        }
    }
    go_back_center();
    
}

// void task2_run(){
//     if(current_position != 0){go_back_center();current_position = 0;}
//     OpenMV_Turn_Control(-1-current_direction,20);
//     OpenMV_Go_Control(20,10,0);     //���ϵ�1
//     OpenMV_Request_Color_Detection(++current_process)
