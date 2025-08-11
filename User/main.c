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
#include "MoveSet.h"
#include "led.h"
#include "Task.h"
void init(){
    Stm32_Clock_Init(7);      
    delay_init(8);
    OLED_Init();
    PCA9685_Init();
    MOTOR_Init();
    Usart2_Init(115200);
    Usart5_Init(115200);   
    OpenMV_Control_Init();
    Encoder_Init();
    PID_Control_Init();
    LED_Init();
    LED_ON(2);
    // crazyMe_Multi(servo_init,10,20,30);
    delay_ms(3000);

}


int main(){
    init();
//    task1_run();
//    task2_run();





            OpenMV_Request_Color_Detection(3);

                       OpenMV_Turn_Control(3,20);
delay_ms(5000);
            OpenMV_Request_Color_Detection(3);
            OpenMV_Turn_Control(3,20);






//    auto_catch();
//    OpenMV_Turn_Control(3,20);

// OpenMV_Go_Control(40,2,0.6); //直接前往F,加颜色识别，第一次抓取
// delay_ms(1000);
// Position_Control_Start_All(0.32,40);


// OpenMV_Go_Control(40,2,0.94); //直接前往F,无颜色识别第一次抓取



// crazyMe_Multi(servo_1,2,20,30);//第一次抓取
// crazyMe_Multi(servo_2,2,20,30);
// Position_Control_Start_All(-0.45,40);
// delay_ms(500);
// crazyMe_Multi(servo_5,2,20,30);
// OpenMV_Turn_Control(3,20);

// OpenMV_Go_Control(40,2,1.05); //直接前往F,无颜色识别，第二次抓取
// delay_ms(500);
// crazyMe_Multi(servo_3,2,20,30);//第二次抓取
// crazyMe_Multi(servo_4,2,20,30);
// Position_Control_Start_All(-0.2,40);
// delay_ms(300);
// crazyMe_Multi(servo_5,2,20,30);
// Position_Control_Start_All(1.2,40);
// delay_ms(500);
// crazyMe(9,160,160,20,30);//抓取物块
// Position_Control_Start_All(-1.7,40);
// delay_ms(2000);
// OpenMV_Turn_Control(3,20);
    while(1){ 
      }
  
  
  }
