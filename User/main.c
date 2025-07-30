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
    // ϵͳʱ�ӳ�ʼ�����ж����ȼ���������
    Stm32_Clock_Init(7);  // 72MHzʱ�ӣ������ж����ȼ�����2
    
    delay_init(8);
    OLED_Init();
    PCA9685_Init();
    MOTOR_Init();
    Usart2_Init(115200);
    Usart5_Init(115200);   
    OpenMV_Control_Init();
    Encoder_Init();
    PID_Control_Init();
}


int main(){
    init();
    


    OpenMV_Go_Control(50,3,0.95);


      while(1){

      }
  
  
  } 