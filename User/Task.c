#include "Task.h"
#include "openmv_control.h"
#include "oled.h"
#include "delay.h"
#include "MoveSet.h"
#include "PCA9685.h"
static int8_t current_direction = 0;
static int8_t current_position = 0;
static int8_t current_process = 0;
static int8_t sleeve_flag = 0;
static int8_t claw_flag_right = 0;
static int8_t claw_flag_left = 0;
static int8_t map_flag [5] = {1,0,1,0,1};
static int8_t end_flag [5] = {0,0,0,0,0};

void task1_run(){
    OpenMV_Go_Control(50,3,0); //ֱ��ǰ��C��
    current_direction = 3;
    OpenMV_Request_Color_Detection(current_direction-1);
    Position_Control_Start_All(0.7,40);
    delay_ms(1000);
    crazyMe(9,160,160,20,30);//ץȡ���
    current_position = 1;
    sleeve_flag = OpenMV_Get_Color(current_direction-1);

    OLED_ShowNum(1, 0, sleeve_flag, 3);

    map_flag[current_direction-1] = 0;
    if(current_direction == sleeve_flag){ //��������ɫ��ȷ����ֱ��ǰ���������
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
        if(sleeve_flag||claw_flag_right){ //�����ǰ��Ͳ����צ�������
            if(!sleeve_flag&&claw_flag_right){ // �����ǰ��צ�ӣ����佻����Ͳ
                OpenMV_Turn_Control(claw_flag_right-current_direction,20); // ת������Ӧ��ɫ�ķ���
                current_direction = claw_flag_right;
                crazyMe(0,160,160,20,30);
                crazyMe(2,90,90,20,30);
                crazyMe(0,90,90,20,30);
                Position_Control_Start_All(0.5,40);
                crazyMe(9,160,160,20,30);//ץȡ���
                sleeve_flag = claw_flag_right;
                claw_flag_right = 0;
            }
            else {
                OpenMV_Turn_Control(sleeve_flag-current_direction,20); // ת������Ӧ��ɫ�ķ���
                current_direction = sleeve_flag;
            }
            if(map_flag[current_direction-1]){ //�����ͼ������飬����צ�ӹ�ȡ
                OpenMV_Go_Control(50,1,0); //ǰ�����
                OpenMV_Request_Color_Detection(current_direction-1);
                Position_Control_Start_All(0.6,40);
                delay_ms(1000);
                crazyMe(0,160,160,20,30);
                crazyMe(2,23,23,20,30);
                crazyMe(0,20,20,20,30);
                map_flag[current_direction-1] = 0;
                claw_flag_right = OpenMV_Get_Color(current_direction-1);

                OLED_ShowNum(1, 0, claw_flag_right, 3);

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
            for(int i = 0; i < 5; i++){ //������ͼ���ҵ����
                if(map_flag[i]){
                    OpenMV_Turn_Control(i-current_direction+1,20); //ת���������ķ���
                    current_direction = i+1;
                    break;
                }
            }
            OpenMV_Go_Control(50,1,0); //ǰ�����
            OpenMV_Request_Color_Detection(current_direction-1);
            Position_Control_Start_All(0.7,40);
            delay_ms(1000);
            crazyMe(9,160,160,20,30);//ץȡ���            
            current_position = 1;
            sleeve_flag = OpenMV_Get_Color(current_direction-1);

            OLED_ShowNum(1, 0, sleeve_flag, 3);

            map_flag[current_direction-1] = 0;            
            if(current_direction == sleeve_flag){ //��������ɫ��ȷ����ֱ��ǰ���������
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }
        }
    }
    go_back_center();
    current_position = 0;
    current_process = 0;
}




void task2_run() {
    if (current_position != 0) {
        go_back_center();
        current_position = 0;
    }
    while (current_process < 10) {
        if (current_direction == sleeve_flag) {
            // ֱ�ӷ���
        } else if (current_direction == claw_flag_right && sleeve_flag == 0) { // ��צ����
            crazyMe(0, 160, 160, 20, 30);
            crazyMe(2, 90, 90, 20, 30);
            crazyMe(0, 90, 90, 20, 30);
            Position_Control_Start_All(0.5, 40);
            crazyMe(9, 160, 160, 20, 30); // ץȡ���
            sleeve_flag = claw_flag_right;
            claw_flag_right = 0;
        } else if (current_direction == claw_flag_left && sleeve_flag == 0) {// ��צ����
            crazyMe(0, 160, 160, 20, 30);
            crazyMe(2, 90, 90, 20, 30);
            crazyMe(0, 90, 90, 20, 30);
            Position_Control_Start_All(0.5, 40);
            crazyMe(9, 160, 160, 20, 30); // ץȡ���
            sleeve_flag = claw_flag_left;
            claw_flag_left = 0;
        } else {
            if (sleeve_flag != 0) {
                OpenMV_Turn_Control(sleeve_flag - current_direction, 20);
                current_direction = sleeve_flag;
            } else if (claw_flag_right != 0) {
                OpenMV_Turn_Control(claw_flag_right - current_direction, 20);
                current_direction = claw_flag_right;
            } else if (claw_flag_left != 0) {
                OpenMV_Turn_Control(claw_flag_left - current_direction, 20);
                current_direction = claw_flag_left;
            } else {
                auto_catch();
            }
        }

        if (current_process < 6) {
            if (end_flag[current_direction-1]) {
                OpenMV_Go_Control(50, 1, 0.5); // ǰ�����
                current_position = 1;
                crazyMe(9, 46, 46, 20, 30); // �������
                sleeve_flag = 0;
                current_process++;
            } else {
                OpenMV_Go_Control(50, 2, 0.62); // �����ͼ��û����飬��ֱ��ǰ���������
                current_position = 2;
                crazyMe(9, 46, 46, 20, 30); // �������
                sleeve_flag = 0;
                current_process++;
            }
        } else {
            if (end_flag[current_direction-1]) {
                // ������
            } else {
                OpenMV_Go_Control(50, 2, 0.3); 
                current_position = 2;
                crazyMe(9, 90, 90, 20, 30); // ��Ͳ΢��

                sleeve_flag = 0;

            }
        }
    }
}

void auto_catch(){
    if(current_process<5){
        OpenMV_Turn_Control(-current_direction,20);
        current_direction = 0;
    }
    else {
        OpenMV_Turn_Control(6-current_direction,20);
        current_direction = 6;
    }
    if(current_process==0){
        OpenMV_Go_Control(50,2,0.2);     //���ϵ�1
        current_position = 2;
        OpenMV_Request_Color_Detection(1);
        Position_Control_Start_All(0.3,40);
            //צ�Ӷ���
        claw_flag_right = OpenMV_Get_Color(0);//A
        claw_flag_left = OpenMV_Get_Color(1);//B
        Position_Control_Start_All(-0.3,40);
        go_back_center();
    }
    if(current_process==5){
        OpenMV_Go_Control(50,2,0.3);     //���ϵ�2
        current_position = 2;
        OpenMV_Request_Color_Detection(3);
        //צ�Ӷ���
        claw_flag_right = OpenMV_Get_Color(1);//B
        claw_flag_left = OpenMV_Get_Color(0);//A
        Position_Control_Start_All(-0.3,40);
        go_back_center();
    }
    else {
        OpenMV_Go_Control(50,2,0);     //���ϵ�1/2
        current_position = 2;
        //צ�Ӷ���
        if(current_process==2){
            claw_flag_right = OpenMV_Get_Color(4);//E
            claw_flag_left = OpenMV_Get_Color(2);//C            
        }
        else {
            claw_flag_right = OpenMV_Get_Color(2);//C
            claw_flag_left = OpenMV_Get_Color(4);//E  
        }
        sleeve_flag = OpenMV_Get_Color(3);//D
        Position_Control_Start_All(-0.3,40);
        go_back_center();
    }    

}

void go_back_center(){
    if(current_position == 0) return;
    else if(current_position == 1){Position_Control_Start_All(-1.45,40);delay_ms(1500);}
    else if(current_position == 2){
        Position_Control_Start_All(-1.2,40);
        delay_ms(1000);
        OpenMV_Turn_Control(2,20);
        current_direction = current_direction+4;
        if (current_direction>7){current_direction -= 8;}
        delay_ms(300);
        OpenMV_Go_Control(50,1,1.65); // ǰ������
        delay_ms(1000);
    }
}
