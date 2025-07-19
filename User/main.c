#include "stm32f10x.h"
#include "sys.h"
#include "delay.h"
#include "Usart1.h"
#include "oled.h"
#include "PCA9685.h"
#include "TB6612.h"
#include "Usart5.h"


void init(){
    delay_init(8);
    OLED_Init();
    PCA9685_Init();
    MOTOR_Init();
    Usart5_Init(9600);
    Encoder_Init();
    Motor_Cascade_PID_Init();
}


int main(){

    init();

    // Motor_Set_Target_With_Speed(Motor1, 7, 1);
    // Motor_Set_Target_With_Speed(Motor2, 7, 1);
    // Motor_Set_Target_With_Speed(Motor3, 7, 1);
    // Motor_Set_Target_With_Speed(Motor4, 7, 1);
    // TurnGo(0.845,10);

    crazyMe(1,50,50,50,50);
    while(1){
    //    OLED_ShowSignedNum(1, 1, Get_Accumulated_Position(Motor1), 7);
    //    OLED_ShowSignedNum(2, 1, Get_Accumulated_Position(Motor2), 7);
    //    OLED_ShowSignedNum(3, 1, Get_Accumulated_Position(Motor3), 7);
    //    OLED_ShowSignedNum(4, 1, Get_Accumulated_Position(Motor4), 7);
    }
}