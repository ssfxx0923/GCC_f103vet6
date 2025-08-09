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

    OpenMV_Go_Control(40,3,0); //直接前往C点
    current_direction = 3;
    delay_ms(1000);
    OpenMV_Request_Color_Detection(current_process);
    //抓取物块
    sleeve_flag = 1;
    map_flag[current_direction] = 0;
    if(current_direction == OpenMV_Get_Color(current_process)){ //如果物块颜色正确，则直接前往放置物块
        OpenMV_Go_Control(20,10,0);
        sleeve_flag = 0;
        end_flag[current_position] = 1;
    }
    else{
        OpenMV_Go_Control(20,10,0); //如果物块颜色不正确，则返回中心
        current_position = 0;
    }
    for(int loop = 0; loop < 2; loop++){
        if(current_position != 0){go_back_center();current_position = 0;}
        if(sleeve_flag||claw_flag){ //如果当前套筒或者爪子有物块
            OpenMV_Turn_Control(OpenMV_Get_Color(current_process)-current_direction,20); // 转动到对应颜色的方向
            current_direction = OpenMV_Get_Color(current_process);
            if(!sleeve_flag&&claw_flag){ // 如果当前是爪子，将其交给套筒
                OpenMV_Go_Control(20,10,0);
                sleeve_flag = 1;
                claw_flag = 0;
            }
            if(map_flag[current_direction]){ //如果地图上有物块，则用爪子勾取
                
                OpenMV_Request_Color_Detection(++current_process);
                OpenMV_Go_Control(20,10,0);
                map_flag[current_direction] = 0;
                claw_flag = 1;
                current_position = 1;

                OpenMV_Go_Control(20,10,0); //前往放置物块
                current_position = 2;
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }
            else{
                OpenMV_Go_Control(20,10,0); //如果地图上没有物块，则直接前往放置物块
                current_position = 2;
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }

        }
        else{   //如果当前套筒爪子都没有物块
            for(int i = 0; i < 5; i++){ //遍历地图，找到物块
                if(map_flag[i]){
                    OpenMV_Turn_Control(++i-current_direction,20); //转动到有物块的方向
                    current_direction = i;
                    break;
                }
            }

            OpenMV_Go_Control(20,10,0); //前往物块
            OpenMV_Request_Color_Detection(++current_process);
            current_position = 1;
            sleeve_flag = 1;
            map_flag[current_direction] = 0;
            
            if(current_direction == OpenMV_Get_Color(current_process)){ //如果物块颜色正确，则直接前往放置物块
                OpenMV_Go_Control(20,10,0);
                sleeve_flag = 0;
                end_flag[current_position] = 1;
            }
            else{ //如果物块颜色不正确，则返回中心
                go_back_center();
                current_position = 0;
            }


        }
    }
    
}

// void task2_run(){
//     if(current_position != 0){go_back_center();current_position = 0;}
//     OpenMV_Turn_Control(-1-current_direction,20);
//     OpenMV_Go_Control(20,10,0);     //物料点1
//     OpenMV_Request_Color_Detection(++current_process)
