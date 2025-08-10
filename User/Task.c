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
        OpenMV_Go_Control(50,1,1.65); // 前往中心
        delay_ms(1000);
    }
}




void task1_run(){

    OpenMV_Go_Control(50,3,0); //直接前往C点
    current_direction = 3;
    OpenMV_Request_Color_Detection(current_block);
    OLED_ShowNum(1,5,OpenMV_Get_Color(current_block),2);
    Position_Control_Start_All(0.7,40);
    delay_ms(1000);
    crazyMe(9,160,160,20,30);//抓取物块
    current_position = 1;
    sleeve_flag = 1;
    map_flag[current_direction-1] = 0;
    if(current_direction == OpenMV_Get_Color(current_block)){ //如果物块颜色正确，则直接前往放置物块
        Position_Control_Start_All(0.3,40);
        OpenMV_Go_Control(50,1,0.62);
        current_position = 2;
        crazyMe(9,46,46,20,30);//放下物块
        sleeve_flag = 0;
        end_flag[current_position-1] = 1;
        current_process++;
    }
    while(current_process < 3){     //每次放置加一，最多放置三个放置物块
        if(current_position != 0){go_back_center();current_position = 0;}
        if(sleeve_flag||claw_flag){ //如果当前套筒或者爪子有物块
            OpenMV_Turn_Control(OpenMV_Get_Color(current_block)-current_direction,20); // 转动到对应颜色的方向
            current_direction = OpenMV_Get_Color(current_block);
            if(!sleeve_flag&&claw_flag){ // 如果当前是爪子，将其交给套筒
                Position_Control_Start_All(0.5,40);
                sleeve_flag = 1;
                claw_flag = 0;
            }
            if(map_flag[current_direction-1]){ //如果地图上有物块，则用爪子勾取
                OpenMV_Go_Control(50,1,0); //前往物块
                OpenMV_Request_Color_Detection(++current_block);
                Position_Control_Start_All(0.6,40);
                delay_ms(1000);
//                crazyMe(9,160,160,20,30);//抓取物块
                map_flag[current_direction-1] = 0;
                claw_flag = 1;
                current_position = 1;
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//放下物块
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }
            else{
                OpenMV_Go_Control(50,2,0.62); //如果地图上没有物块，则直接前往放置物块
                current_position = 2;
                crazyMe(9,46,46,20,30);//放下物块
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }

        }
        else{   //如果当前套筒爪子都没有物块
            OLED_ShowNum(4,4,current_direction,3);
            for(int i = 0; i < 5; i++){ //遍历地图，找到物块
                if(map_flag[i]){
                    OpenMV_Turn_Control(++i-current_direction,20); //转动到有物块的方向
                    current_direction = i;
                    break;
                }
            }
            OpenMV_Go_Control(50,1,0); //前往物块
            OpenMV_Request_Color_Detection(++current_block);
            Position_Control_Start_All(0.7,40);
            delay_ms(1000);
            crazyMe(9,160,160,20,30);//抓取物块            
            current_position = 1;
            sleeve_flag = 1;
            map_flag[current_direction-1] = 0;            
            if(current_direction == OpenMV_Get_Color(current_block)){ //如果物块颜色正确，则直接前往放置物块
                Position_Control_Start_All(0.3,40);
                OpenMV_Go_Control(50,1,0.62);
                current_position = 2;
                crazyMe(9,46,46,20,30);//放下物块
                sleeve_flag = 0;
                end_flag[current_position-1] = 1;
                current_process++;
            }
            else{ //如果物块颜色不正确，则返回中心
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
//     OpenMV_Go_Control(20,10,0);     //物料点1
//     OpenMV_Request_Color_Detection(++current_process)
