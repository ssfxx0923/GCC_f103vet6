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
    



  // crazyMe_Multi(servo_1,10,10,10);

    // delay_ms(1000);
    //  crazyMe(8,150,150,10,10);
    // delay_ms(1000);
    // crazyMe(9,46,46,10,10);


 //    OpenMV_Go_Control(40,2,0.95);

  //  Position_Control_Start_All(-4,40);

    // OpenMV_Go_Control(40,2,0.95);


     OpenMV_Go_Control(50,2,1.92);
     delay_ms(2000);
     OpenMV_Turn_Control(0,4,20);
     delay_ms(2000);
     OpenMV_Go_Control(40,2,0.95);
     delay_ms(2000);


  // OpenMV_Go_Control(40,2,1.2);
  //  delay_ms(5000);
  // //  OpenMV_Go_Control(50,3,0.95);
  //   OpenMV_Turn_Control(1,3,8);
  //   OpenMV_Go_Control(40,2,0.6);

 // OpenMV_Send_Command(3);



      while(1){
   //     OpenMV_Line_Forward_Control(20);
  //      OLED_ShowSignedNum(1,1,OpenMV_Get_PID_Output(),5);

      }
  
  
  }
