//#include "sys.h"
//#include "delay.h"
//#include "Usart1.h"
//#include "led.h"
//#include "LobotSerialServo.h"
//#include "PCA9685.h"
//#include "grayscale.h"
//#include "exti.h"
//#include "beep.h"
//#include "Uart4.h"
//#include "Uart5.h"
//#include "Servo.h"
//#include "oled.h"
//#include "MPU6050.h"
//#include "inv_mpu.h"
//#include "TCS34725.h"
//#include "gray_go.h"
//#include "Task_Working.h"
//#include "TB6612.h"
//extern uint32_t Remainder;
//extern uint32_t Divisor;
//extern uint8_t  Dis_Buf1[8];
//extern uint8_t  Dis_Buf2[8];
//extern uint8_t  Dis_Buf3[8];
//extern short myaw;
//extern uint8_t Color_Flag;
//uint8_t Last_Color_Flag=1;
//void Init(void)
//{
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //中断优先级分组
//	delay_init(168);		                            //初始化延时函数
//	
//	LED_Init();		        													//初始化LED端口
//	MOTOR_Init();         													//初始化直流电机
//	Usart1_Init(115200);  													//Openmv初始化
//	Uart4_Init(115200);  					 								  //printf初始化
//	Uart5_Init(9600);     													//蓝牙初始化
//	IIC3_Init();         				 										//IIC3初始化
//	Grayscale_Init();     													//灰度初始化
//	EXTIX_Init();         													//中断初始化
//	BEEP_Init();          													//中断初始化
//	OLED_Init();          													//显示屏初始化
//	MPU_Init();           													//MPU6050初始化（1）
//	mpu_dmp_init();       													//MPU6050初始化（2）
//	TCS34725_GPIO_Init(); 													//TCS34725初始化(1)
//	TCS34725_Init();      													//TCS34725初始化(2)
//	integrationTime(33);  													//TCS34725初始化(3)
//	PCA9685_Init();       													//PCA9685初始化
//	Now_Position_Init();
//}

///*
//调试车子任务代码之前，应先测试一下几个模块：
//1.八路灰度寻迹
//2.一路灰度卡位置
//3.转角是否平滑正确
//4.舵机控制是否平稳
//5.颜色识别是否正确
//*/

//void Try(void)
//{
//	uint8_t i,j=0;
////		Task1();
////	  Task2();
////	Set_want_Position(BEGIN);

///****************OLED测试***********************/	
////		while(1)
////		{
////			OLED_Refresh();
////			OLED_ShowChinese(80,80,5,16,0);			
////		}
///****************寻迹测试***********************/	
////	while(1)
////	{
////	   anti_SearchRun();		
////	}
////	
////	while(1)
////	{
////	   SearchRun();		
////	}
////	
///*****************一路灰度卡位置测试************************/
////	ALL_Count(SearchRun,6,L_F);
////	ALL_Count(SearchRun,5,L_C);

////      L_Front_Count(SearchRun,6*2);
////	    delay_ms(1000);
////      L_Front_Count(anti_SearchRun,6*2);
//	
////      L_Centre_Count(SearchRun,6*2);
////	    delay_ms(1000);
////      L_Centre_Count(anti_SearchRun,6*2);

////      L_Back_Count(SearchRun,6*2);
////	    delay_ms(1000);
////      L_Back_Count(anti_SearchRun,6*2);

///*****************转角测试************************/
////Now_Position_Init();

////Set_want_Position(AA);
////delay_ms(1000);
////Set_want_Position(BB);
////delay_ms(1000);
////Set_want_Position(CC);
////delay_ms(1000);
////Set_want_Position(DD);
////delay_ms(1000);
////Set_want_Position(EE);
////delay_ms(1000);
////Set_want_Position(FF);
////delay_ms(1000);
////Set_want_Position(HH);
////delay_ms(1000);
////Set_want_Position(WRITE);
////delay_ms(1000);
////Set_want_Position(ALL1);
////delay_ms(1000);
////Set_want_Position(RED);
////delay_ms(1000);
////Set_want_Position(ALL2);
////delay_ms(1000);
////Set_want_Position(GREEN);
////delay_ms(1000);
////Set_want_Position(BLACK);
////delay_ms(1000);
////Set_want_Position(BEGIN);




//////		ALL_Count(SearchRun,5,L_C);
//////		Set_Motor(25,25);
//////		delay_ms(100);
//////    Set_Motor(0,0);	

//////			ALL_Count(SearchRun,6,L_F);
//////      delay_ms(500);
//////			
//////			ALL_Count(anti_SearchRun,1,L_C);
//////			Set_Motor(-30,-30);
//////			delay_ms(120);
//////      Set_Motor(0,0);
//////      delay_ms(1000);
//////			
//////			Car_TurnToAngle4(135);
//////      delay_ms(1000);			
//////			Car_TurnToAngle4(0);
//////      delay_ms(1000);			
//////			Car_TurnToAngle4(-135);
//////      delay_ms(1000);			
//////			Car_TurnToAngle4(-90);
//////      delay_ms(1000);
//////			Car_TurnToAngle4(45);
//////      delay_ms(1000);			
//////			Car_TurnToAngle4(90);
//////      delay_ms(1000);
//////			Car_TurnToAngle4(-45);	
//////			
//////			
//////			Car_TurnToAngle4(0);
//////      delay_ms(1000);			


//////			ALL_Count(anti_SearchRun,5,L_C);			
////			
////			
//////			delay_ms(1000);
//////			Car_TurnToAngle4(0);
//////		 Set_Angle(Red);
//////			Go_Home();		 
////		
//////			Car_TurnToAngle4(0);
//////			Car_TurnToAngle4(45);
//////			Car_TurnToAngle4(90);
//////			Car_TurnToAngle4(135);
//////			Car_TurnToAngle4(-45);
//////			Car_TurnToAngle4(-90);
//////			Car_TurnToAngle4(-135);

/////*****************舵机测试************************/
////  DuoJi_Init();
////	Claw_Action(Ton_Catch_1);	
//////	ALL_Count(SearchRun,4,L_F);
//////  Claw_Action(Put_Up);
////	TIME_GO(2);

////	crazyMe(Sleeve_Servo,105,40,5,80);//套筒上去				
////	TIME_GO(2);
////	Claw_Action(Ton_CatchTwo_2);



////	crazyMe(1,50,50,5,80);//爪子张开	
////	delay_ms(1500);		
////	crazyMe(2,93,93,5,80);//套筒下来				
//	
////	delay_ms(1500);
////	crazyMe(2,44,44,5,80);//套筒上去
////  delay_ms(1000);
////  crazyMe(1,128,90,5,80);//爪子张开
////	delay_ms(1500);
////	crazyMe(2,93,93,5,80);//套筒下来		
////  delay_ms(1500);
////	crazyMe(1,90,128,5,80);//套筒抓
//////		for(i=0;i<38;i++)
//////		{
//////	     crazyMe(1,90,90+i,5,80);//套筒抓
//////       delay_ms(2);			
//////		}
////	delay_ms(1500);
////	crazyMe(2,44,44,5,80);//套筒上去
////  delay_ms(1000);
////  crazyMe(1,128,90,5,80);//爪子张开
////	delay_ms(1500);
////	crazyMe(2,93,93,5,80);//套筒下来		
////  delay_ms(1500);	
////	crazyMe(1,90,128,5,80);//套筒抓

////		crazyMe(1,50,128,5,80);//套筒抓

////	crazyMe(1,50,50,5,80);//爪子张开
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//套筒上去
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//套筒下来		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//套筒抓
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//套筒上去
////	delay_ms(1000);
////	crazyMe(1,50,50,5,80);//爪子张开
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//套筒下来		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//套筒抓
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//套筒上去
////	delay_ms(1000);
////	crazyMe(1,50,50,5,80);//爪子张开
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//套筒下来		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//套筒抓

//////Now_Position_Init();

////Set_want_Position(CC);
////delay_ms(1500);
////crazyMe(1,50,128,5,80);
////delay_ms(1500);
////Set_want_Position(AA);
////delay_ms(1500);
////	crazyMe(15,90,90,5,80);
//////delay_ms(500);
////	crazyMe(4,100,100,5,80);
//////delay_ms(500);
////	crazyMe(15,40,40,5,80);
//////delay_ms(500);
////	crazyMe(4,174,174,5,80);
//////delay_ms(600);
////	crazyMe(15,40,90,5,80);







////  crazyMe(15,49,49,5,80);
////	delay_ms(500);		
////	crazyMe(4,145,145,5,80);
//// 	delay_ms(500);
////  crazyMe(15,49,90,5,80);
////    delay_ms(500);
////		crazyMe(8,56,56,5,80);
////		delay_ms(500);
////		crazyMe(0,33,33,5,80);
////		delay_ms(500);
////		crazyMe(8,56,25,5,80);		
////Set_want_Position(HH);

////		crazyMe(15,90,38,5,80);    
////		crazyMe(4,80,80,5,80);
////		crazyMe(15,90,90,5,80);		

////		Set_Motor(20,20);
////		delay_ms(400);
////    Set_Motor(0,0);
////		
////		delay_ms(1500);
////		crazyMe(1,50,128,5,80);








////		crazyMe(15,60,60,5,80);
////		delay_ms(1000);
////		crazyMe(4,145,145,5,80);
////		delay_ms(1000);
////		crazyMe(15,60,90,5,80);
////		delay_ms(1000);
////		
////		crazyMe(8,47,47,5,80);
////		delay_ms(1000);
////		crazyMe(0,33,33,5,80);
////		delay_ms(1000);
////		crazyMe(8,44,25,5,80);		

////ALL_Count(SearchRun,1,L_F);
////		Set_Motor(20,20);
////		delay_ms(400);
////    Set_Motor(0,0);					
////		delay_ms(1500);
////		crazyMe(1,50,128,5,80);


//		 Claw_Action(Put_Up);		





////Set_want_Position(ALL1);
////		Set_Motor(30,30);
////		delay_ms(280);
////    Set_Motor(0,0);							

////		delay_ms(600);		
////		crazyMe(15,60,60,5,80);
////		crazyMe(4,145,145,5,80);
////		crazyMe(15,60,90,5,80);
////		crazyMe(8,47,47,5,80);
////		crazyMe(0,33,33,5,80);
////		crazyMe(8,44,25,5,80);		
////ALL_Count(SearchRun,1,L_F);
////		Set_Motor(20,20);
////		delay_ms(400);
////    Set_Motor(0,0);					
////		crazyMe(1,50,128,5,80);
////		delay_ms(600);




////  delay_ms(1500);
////	crazyMe(2,93,93,5,80);//套筒下来			
////	delay_ms(1500);
////	crazyMe(1,128,50,5,80);//套筒放	
////	delay_ms(1500);
////	crazyMe(2,46,46,5,80);//套筒上去


////	crazyMe(15,0,45,5,80);		

////		
///*****************颜色识别测试************************/
//// while(1)
//// {
////	 OLED_Refresh();
////	 Dis_Temp();
//////	 See_Color();
////	 delay_ms(100);
//// }
///*****************显示屏和灰度测试****************/	
////	while(1)
////	{
////		OLED_Refresh();		
////		OLED_ShowChinese(80,80,5,16,0);
////		
////		OLED_ShowNum(1,1,F1,2,16,1);
////		OLED_ShowNum(1,16,F2,2,16,1);
////		OLED_ShowNum(1,32,F3,2,16,1);
////		OLED_ShowNum(1,48,F4,2,16,1);
////		OLED_ShowNum(24,1,F5,2,16,1);
////		OLED_ShowNum(24,16,F6,2,16,1);
////		OLED_ShowNum(24,32,F7,2,16,1);
////		OLED_ShowNum(24,48,F8,2,16,1);
//		
//////		OLED_ShowNum(1,1,D1,2,16,1);
//////		OLED_ShowNum(1,16,D2,2,16,1);
//////		OLED_ShowNum(1,32,D3,2,16,1);
//////		OLED_ShowNum(1,48,D4,2,16,1);
//////		OLED_ShowNum(24,1,D5,2,16,1);
//////		OLED_ShowNum(24,16,D6,2,16,1);
//////		OLED_ShowNum(24,32,D7,2,16,1);
//////		OLED_ShowNum(24,48,D8,2,16,1);				
////	}
///*****************车子起步开始测试***************/	
////	   Go_Head(Begin_ToX);	 											/*从出发点到C点*/
////		 delay_ms(500);
////	   Go_Head(X_Totarget);	 											
////     delay_ms(500);
////		 Go_Back(Target_ToCenter);
////		 Set_Angle(Blue);
////	   Go_Head(Center_ToX);	 											
////	   Go_Head(X_Totarget);	 											
////     delay_ms(500);
////		 Go_Back(Target_ToCenter);
////		 Set_Angle(Red);
////     delay_ms(500);
////		 Set_Angle(Green);
////	   Go_Head(Center_ToX);	 											
////	   Go_Head(X_Totarget);	 											
////     delay_ms(500);
////		 Go_Back(Target_ToCenter);
////		 Set_Angle(Red);
////		 Go_Back(Target_ToCenter);
////		 L_Centre_Count(anti_SearchRun,4*2-1);		 
//}

///*
//以下为接线提示：
//pac9685:IIC3  
//TCS345: IIC1
//八路灰度前：F
//八路灰度后：G
//电机左 1：左后 电机右 2：左前
//电机左 3：右后 电机右 4：右前
//L_front   PC1
//L_Centre  PC2
//L_back    PC3
//R_front   PC0
//R_Centre  PF11
//R_back    PB1
//*/

////任务一：抓取流程是C->E->A
//void Task1(void)
//{
//	uint8_t Case,Now_Color;
//	uint8_t DoneClock;
//	DuoJi_Init();
//	Set_want_Position(CC);
//	for(DoneClock=0;DoneClock<3;DoneClock++)
//	{
//		delay_ms(1000);
//		Now_Color=See_Color();									/*识别颜色*/	
//		if(Color_Flag==0)                       /*预防没有识别到颜色，则向前推物块再次识别*/
//		{
//			TIME_GO(2);
//			delay_ms(1000);		
//			Now_Color=See_Color();								/*识别颜色*/				
//		}
//		Case=Judge_Case(Now_Color,DoneClock);		/*判断情况*/
//		Task1_Working(Case,Now_Color);					/*对挡或者不挡情况执行相应任务*/				
//	}
//}
////任务二：先识别B并且确定好所有颜色的顺序
//void Task2(void)
//{
//	//先把颜色确定完再分别前往两堆将东西爪到相应的位置
//	uint8_t HaveDone;
//	for(HaveDone=0;HaveDone<4;HaveDone++)
//	{
//		Task2_Working(HaveDone);		
//	}
//}

//int main(void)
//{
//	
////	Init();
////	Task1();
////	Task2();
////	Set_want_Position(BLACK);

////	delay_ms(5000);
////	Set_want_Position(BEGIN);
////	delay_ms(5000);
////	Set_want_Position(BLACK);
////	OLED_ShowNum(1,1,5,2,16,1);
//	//beep(2);
////	Uart5_SendChar('A');
//	uart4_send_char('A');
////	See_Color();
//	//beep(i);
////	delay_ms(2000);
////	beep(3);
//    BEEP_Init();
//	Beep_one();
//	while(1);

////  Try();
//}

