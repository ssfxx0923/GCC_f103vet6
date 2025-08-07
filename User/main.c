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
    delay_ms(5000);
}


int main(){
    init();
    

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
      
    GPIO_InitTypeDef GPIO_InitStructure;
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(GPIOC, &GPIO_InitStructure);
    
    // 设置PC1为高电平
    GPIO_SetBits(GPIOC, GPIO_Pin_1);


    // OpenMV_Go_Control(50,2,1.6);
    // delay_ms(3000);
    // Position_Control_Start_All(0.5,40);
    // delay_ms(3000);
    // Position_Control_Start_All(0.6,40);
    // delay_ms(3000);
    // OpenMV_Go_Control(40,1,0.6);
    // delay_ms(3000);
    // Position_Control_Start_All(-1.6,30); 
    // delay_ms(3000);
    // OpenMV_Turn_Control(1,2,20);
    // delay_ms(3000);
    // OpenMV_Go_Control(40,1,1.2);
    

      while(1){

      }
  
  
  }
