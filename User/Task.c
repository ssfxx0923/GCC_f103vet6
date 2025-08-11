#include "Task.h"
#include "openmv_control.h"
#include "oled.h"
#include "delay.h"
#include "MoveSet.h"
#include "PCA9685.h"
#include "ServoMoveSet.h"
static int8_t current_direction = 3;
static int8_t current_position = 0;
static int8_t current_process = 0;
static int8_t sleeve_flag = 0;
static int8_t claw_flag_right = 0;
static int8_t claw_flag_left = 0;
static int8_t map_flag [5] = {1,0,1,0,1};
static int8_t end_flag [5] = {0,0,0,0,0};

void task1_run(){
    crazyMe_Multi(servo_init,10,20,30);
    OpenMV_Go_Control(50,3,0); //ֱ��ǰ��C��
    current_direction = 3;
    delay_ms(500);
    OpenMV_Request_Color_Detection(current_direction-1);
    delay_ms(500);
    Position_Control_Start_All(0.8,40);
    delay_ms(1000);
    crazyMe(9,160,160,20,30);//ץȡ���
    current_position = 1;
    sleeve_flag = OpenMV_Get_Color(current_direction-1);
    map_flag[current_direction-1] = 0;
    if(current_direction == sleeve_flag){ //��������ɫ��ȷ����ֱ��ǰ���������
        Position_Control_Start_All(0.3,40);
        OpenMV_Go_Control(50,1,0.57);
        current_position = 2;
        crazyMe(9,46,46,20,30);//�������
        sleeve_flag = 0;
        end_flag[current_direction-1] = 1;  // 
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
                delay_ms(500);
                OpenMV_Request_Color_Detection(current_direction-1);
                delay_ms(500);
                Position_Control_Start_All(0.45,40);
                delay_ms(1000);
                crazyMe(0,155,155,20,30);
                crazyMe(2,23,23,20,30);
                crazyMe(0,20,20,20,30);
                map_flag[current_direction-1] = 0;
                claw_flag_right = OpenMV_Get_Color(current_direction-1);
                current_position = 1;
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_direction-1] = 1;  // ʹ�÷��������λ�������
                current_process++;
            }
            else{
                OpenMV_Go_Control(50,2,0.62); //�����ͼ��û����飬��ֱ��ǰ���������
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_direction-1] = 1;  // ʹ�÷��������λ�������
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
            delay_ms(500);
            OpenMV_Request_Color_Detection(current_direction-1);
            delay_ms(500);
            Position_Control_Start_All(0.7,40);
            delay_ms(1000);
            crazyMe(9,160,160,20,30);//ץȡ���            
            current_position = 1;
            sleeve_flag = OpenMV_Get_Color(current_direction-1);
            map_flag[current_direction-1] = 0;            
            if(current_direction == sleeve_flag){ //��������ɫ��ȷ����ֱ��ǰ���������
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//�������
                sleeve_flag = 0;
                end_flag[current_direction-1] = 1;  // ʹ�÷��������λ�������
                current_process++;
            }
        }
    }
    go_back_center();
    current_process = 0;
}

void task2_run() {

    while (current_process < 10) {

        if(current_position != 0){go_back_center();current_position = 0;}
        if (current_direction == sleeve_flag&& sleeve_flag > 0) {
            OLED_ShowNum(3, 3, 555, 3);
        } else if ((current_direction == claw_flag_right &&claw_flag_right>0)&&sleeve_flag==0) { // ��צ����
            OLED_ShowNum(3, 3, 666, 3);
            crazyMe(0, 155, 155, 20, 30);
            crazyMe(2, 90, 90, 20, 30);
            crazyMe(0, 90, 90, 20, 30);
            Position_Control_Start_All(0.5, 40);
            crazyMe(9, 160, 160, 20, 30); // ץȡ���
            sleeve_flag = claw_flag_right;
            claw_flag_right = 0;
        } else if ((current_direction == claw_flag_left &&claw_flag_left>0)&&sleeve_flag==0) {// ��צ����
            OLED_ShowNum(3, 3, 777, 3);
            crazyMe(1, 25, 25, 20, 30);
            crazyMe(3, 90, 90, 20, 30);
            crazyMe(1, 90, 90, 20, 30);
            Position_Control_Start_All(0.5, 40);
            crazyMe(9, 160, 160, 20, 30); // ץȡ���
            sleeve_flag = claw_flag_left;
            claw_flag_left = 0;
        } else {
            OLED_ShowNum(3, 3, 888, 3);
            if (sleeve_flag > 0) {
                OpenMV_Turn_Control(sleeve_flag - current_direction, 20);
                current_direction = sleeve_flag;
            } else if (claw_flag_right > 0) {
                OpenMV_Turn_Control(claw_flag_right - current_direction, 20);
                current_direction = claw_flag_right;
            } else if (claw_flag_left > 0) {
                OpenMV_Turn_Control(claw_flag_left - current_direction, 20);
                current_direction = claw_flag_left;
            } else {
                auto_catch();
            }
            continue;
        }

        if (current_process < 6) {
            if (end_flag[current_direction-1]) {
                OLED_ShowNum(1,1,current_direction, 2);             
                OLED_ShowNum(1,6,sleeve_flag, 3);
                OLED_ShowNum(1,10,end_flag[current_direction-1], 3);
                OpenMV_Go_Control(50, 1, 0.5); // ǰ�����
                current_position = 1;
                crazyMe(9, 46, 46, 20, 30); // �������
                sleeve_flag = 0;
                current_process++;
                OLED_ShowNum(3,6,current_process,2);
            } else {
                OLED_ShowNum(1,1,current_direction, 3);             
                OLED_ShowNum(1,6,sleeve_flag, 3);
                OLED_ShowNum(1,10,end_flag[current_direction-1], 2);
                OpenMV_Go_Control(50, 2, 0.62); // �����ͼ��û����飬��ֱ��ǰ���������
                current_position = 2;
                crazyMe(9, 46, 46, 20, 30); // �������
                sleeve_flag = 0;
                current_process++;
                OLED_ShowNum(3,6,current_process,3);
            }
        } else {
            if (end_flag[current_direction-1]) {
                // ������
            } else {
                OpenMV_Go_Control(50, 2, 0.3); 
                current_position = 2;
                crazyMe(9, 90, 90, 20, 30); // ��Ͳ΢��              
                crazyMe(9, 90, 90, 20, 30); // ��Ͳ΢��
                sleeve_flag = 0;

            }
        }
    }
}


void auto_catch(){
    OLED_ShowNum(3,6,current_process,3);
    if(current_process<5){
        OpenMV_Turn_Control(-current_direction,20);
        current_direction = 0;
    }
    else {
        OpenMV_Turn_Control(6-current_direction,20);
        current_direction = 6;
    }
    if(current_process==0){
        OpenMV_Go_Control(40,2,0.6);     //���ϵ�1
        current_position = 3;
        delay_ms(500);
        OpenMV_Request_Color_Detection(1);
        delay_ms(500);
        Position_Control_Start_All(0.35,40);
        delay_ms(300);
        crazyMe_Multi(servo_1,2,20,30);//��һ��ץȡ
        crazyMe_Multi(servo_2,2,20,30);
        Position_Control_Start_All(-0.45,40);
        delay_ms(500);
        crazyMe_Multi(servo_5,2,20,30);
        claw_flag_right = OpenMV_Get_Color(0);//A
        claw_flag_left = OpenMV_Get_Color(1);//B
        OLED_ShowNum(4, 1, claw_flag_left, 3);
        go_back_center();
    }
    else if(current_process==5){
        OpenMV_Go_Control(40,2,0.94); //ֱ��ǰ��F,����ɫʶ���һ��ץȡ
        current_position = 3;
        delay_ms(500);
        crazyMe_Multi(servo_1,2,20,30);//��һ��ץȡ
        crazyMe_Multi(servo_2,2,20,30);
        Position_Control_Start_All(-0.50,40);
        delay_ms(500);
        crazyMe_Multi(servo_5,2,20,30);
        claw_flag_right = OpenMV_Get_Color(1);//B
        claw_flag_left = OpenMV_Get_Color(0);//A
        go_back_center();
    }
    else {
        OpenMV_Go_Control(40,2,1.10); //ֱ��ǰ��F,����ɫʶ�𣬵ڶ���ץȡ
        current_position = 3;
        delay_ms(500);
        crazyMe_Multi(servo_3,2,20,30);//�ڶ���ץȡ
        crazyMe_Multi(servo_4,2,20,30);
        Position_Control_Start_All(-0.2,40);
        delay_ms(300);
        crazyMe_Multi(servo_5,2,20,30);
        Position_Control_Start_All(1.2,40);
        delay_ms(500);
        crazyMe(9,160,160,20,30);//ץȡ���
        Position_Control_Start_All(-1.55,40);
        delay_ms(2000);
        if(current_process==2){
            claw_flag_right = OpenMV_Get_Color(4);//E
            claw_flag_left = OpenMV_Get_Color(2);//C      
            OpenMV_Request_Color_Detection(3);
            delay_ms(500);
        }
        else {
            claw_flag_right = OpenMV_Get_Color(2);//C
            claw_flag_left = OpenMV_Get_Color(4);//E  
        }
        sleeve_flag = OpenMV_Get_Color(3);//D
        OLED_ShowNum(4,4,sleeve_flag,3);
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
        current_direction = (current_direction+4)%8;
        delay_ms(300);
        OpenMV_Go_Control(50,1,1.60); // ǰ������
        delay_ms(1000);
    }
    else if(current_position == 3){
        OpenMV_Turn_Control(3,20);
        current_direction = (current_direction+4)% 8;  
        delay_ms(300);
        OpenMV_Go_Control(50,1,1.60); // ǰ������
        delay_ms(1000);
    }
    current_position = 0;
}
