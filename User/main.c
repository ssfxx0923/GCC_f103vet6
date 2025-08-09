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
    

 //    OpenMV_Go_Control(50,3,0);


    OpenMV_Turn_Control(1,20);
    delay_ms(1000);
    OpenMV_Go_Control(50,1,0);

     OpenMV_Request_Color_Detection(0);
      while(1){
        uint8_t color = OpenMV_Get_Color(0);
        OLED_ShowNum(1,1,color,2);
        delay_ms(100);  
      }
  
  
  }
