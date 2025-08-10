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
//     crazyMe_Multi(servo_init,10,20,30);
//    delay_ms(3000);

}


int main(){
    init();
//    task1_run();
            crazyMe(1, 25, 25, 20, 30);
            crazyMe(3, 90, 90, 20, 30);
            crazyMe(1, 90, 90, 20, 30);

    while(1){ 
      }
  
  
  }
