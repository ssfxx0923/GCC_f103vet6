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
#include "ServoMoveSet.h"
#include "led.h"
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
    delay_ms(5000);

}


int main(){
    init();
    

    // OpenMV_Go_Control(40,2,1.65);
    // delay_ms(2000);
    // OpenMV_Turn_Control(1,3,20);
    // delay_ms(2000);
    // OpenMV_Go_Control(40,2,0.6);

   // Position_Control_Start_All(2.1,40);

   OpenMV_Request_Color_Detection(2);
      while(1){
        uint8_t color = OpenMV_Get_Color(2);
        OLED_ShowNum(1,1,color,2);
        // 显示额外的调试信息
        OLED_ShowNum(2,1,color,2);  // 第二行也显示颜色值
        delay_ms(100);  // 稍作延迟以便观察
      }
  
  
  }
