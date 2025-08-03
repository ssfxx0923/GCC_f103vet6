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
 //   delay_ms(5000);
}


int main(){
    init();
    
    // crazyMe(0,5,5,2,100);
    // crazyMe(1,50,50,2,100);
    // crazyMe(2,90,90,2,100);
    // crazyMe(3,5,5,2,100);

    // crazyMe(4,5,5,2,100);
    // crazyMe(5,50,50,2,100);
    // crazyMe(6,90,90,2,100);
    // crazyMe(7,178,178,2,100);

    // crazyMe(8,5,5,2,100);
    // crazyMe(9,50,50,2,100);
    // crazyMe(10,90,90,2,100);
    // crazyMe(11,178,178,2,100);

    // crazyMe(12,5,5,2,100);
    // crazyMe(13,50,50,2,100);
    // crazyMe(14,90,90,2,100);
    // crazyMe(15,178,178,2,100);


 //    OpenMV_Go_Control(40,2,0.95);

  //  Position_Control_Start_All(-4,40);

    // OpenMV_Go_Control(40,2,0.95);


    // OpenMV_Go_Control(50,2,2.12);
    // delay_ms(5000);
    // // OpenMV_Go_Control(50,3,0.95);
    // OpenMV_Turn_Control(1,2,20);
    // OpenMV_Go_Control(40,2,0.95);


  // OpenMV_Go_Control(40,2,1.2);
  //  delay_ms(5000);
  // //  OpenMV_Go_Control(50,3,0.95);
  //   OpenMV_Turn_Control(1,3,8);
  //   OpenMV_Go_Control(40,2,0.6);

 // OpenMV_Send_Command(3);

      while(1){
        OpenMV_Line_Forward_Control(20);
      }
  
  
  }
