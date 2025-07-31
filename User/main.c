#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "Usart1.h"
#include "oled.h"
#include "PCA9685.h"
#include "TB6612.h"
#include "Usart5.h"
#include "pid_control.h"  
#include "openmv_control.h"
#include "Usart2.h"


void init(){
    // 系统时钟初始化和中断优先级分组设置
    Stm32_Clock_Init(7);  // 72MHz时钟，设置中断优先级分组2
    
    delay_init(8);
    OLED_Init();
    PCA9685_Init();
    MOTOR_Init();
    Usart2_Init(115200);
    Usart5_Init(115200);   
    OpenMV_Control_Init();
    Encoder_Init();
    PID_Control_Init();
    delay_ms(5000);
}


int main(){
    init();
    
    // OpenMV_Go_Control(40,2,0.95);

  //  Position_Control_Start_All(-4,40);

    // OpenMV_Go_Control(40,2,0.95);


    // OpenMV_Go_Control(50,2,2.12);
    // delay_ms(5000);
    // // OpenMV_Go_Control(50,3,0.95);
    //  OpenMV_Turn_Control(1,2,20);
    //  OpenMV_Go_Control(40,2,0.95);


    // OpenMV_Go_Control(40,2,1.4);
    // delay_ms(5000);
    // // OpenMV_Go_Control(50,3,0.95);
    //  OpenMV_Turn_Control(1,2,20);
    //  OpenMV_Go_Control(40,2,0.6);



      while(1){
        OpenMV_Line_Backward_Control(0);
      }
  
  
  } 
  