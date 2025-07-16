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
//	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); //�ж����ȼ�����
//	delay_init(168);		                            //��ʼ����ʱ����
//	
//	LED_Init();		        													//��ʼ��LED�˿�
//	MOTOR_Init();         													//��ʼ��ֱ�����
//	Usart1_Init(115200);  													//Openmv��ʼ��
//	Uart4_Init(115200);  					 								  //printf��ʼ��
//	Uart5_Init(9600);     													//������ʼ��
//	IIC3_Init();         				 										//IIC3��ʼ��
//	Grayscale_Init();     													//�Ҷȳ�ʼ��
//	EXTIX_Init();         													//�жϳ�ʼ��
//	BEEP_Init();          													//�жϳ�ʼ��
//	OLED_Init();          													//��ʾ����ʼ��
//	MPU_Init();           													//MPU6050��ʼ����1��
//	mpu_dmp_init();       													//MPU6050��ʼ����2��
//	TCS34725_GPIO_Init(); 													//TCS34725��ʼ��(1)
//	TCS34725_Init();      													//TCS34725��ʼ��(2)
//	integrationTime(33);  													//TCS34725��ʼ��(3)
//	PCA9685_Init();       													//PCA9685��ʼ��
//	Now_Position_Init();
//}

///*
//���Գ����������֮ǰ��Ӧ�Ȳ���һ�¼���ģ�飺
//1.��·�Ҷ�Ѱ��
//2.һ·�Ҷȿ�λ��
//3.ת���Ƿ�ƽ����ȷ
//4.��������Ƿ�ƽ��
//5.��ɫʶ���Ƿ���ȷ
//*/

//void Try(void)
//{
//	uint8_t i,j=0;
////		Task1();
////	  Task2();
////	Set_want_Position(BEGIN);

///****************OLED����***********************/	
////		while(1)
////		{
////			OLED_Refresh();
////			OLED_ShowChinese(80,80,5,16,0);			
////		}
///****************Ѱ������***********************/	
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
///*****************һ·�Ҷȿ�λ�ò���************************/
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

///*****************ת�ǲ���************************/
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

/////*****************�������************************/
////  DuoJi_Init();
////	Claw_Action(Ton_Catch_1);	
//////	ALL_Count(SearchRun,4,L_F);
//////  Claw_Action(Put_Up);
////	TIME_GO(2);

////	crazyMe(Sleeve_Servo,105,40,5,80);//��Ͳ��ȥ				
////	TIME_GO(2);
////	Claw_Action(Ton_CatchTwo_2);



////	crazyMe(1,50,50,5,80);//צ���ſ�	
////	delay_ms(1500);		
////	crazyMe(2,93,93,5,80);//��Ͳ����				
//	
////	delay_ms(1500);
////	crazyMe(2,44,44,5,80);//��Ͳ��ȥ
////  delay_ms(1000);
////  crazyMe(1,128,90,5,80);//צ���ſ�
////	delay_ms(1500);
////	crazyMe(2,93,93,5,80);//��Ͳ����		
////  delay_ms(1500);
////	crazyMe(1,90,128,5,80);//��Ͳץ
//////		for(i=0;i<38;i++)
//////		{
//////	     crazyMe(1,90,90+i,5,80);//��Ͳץ
//////       delay_ms(2);			
//////		}
////	delay_ms(1500);
////	crazyMe(2,44,44,5,80);//��Ͳ��ȥ
////  delay_ms(1000);
////  crazyMe(1,128,90,5,80);//צ���ſ�
////	delay_ms(1500);
////	crazyMe(2,93,93,5,80);//��Ͳ����		
////  delay_ms(1500);	
////	crazyMe(1,90,128,5,80);//��Ͳץ

////		crazyMe(1,50,128,5,80);//��Ͳץ

////	crazyMe(1,50,50,5,80);//צ���ſ�
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//��Ͳ��ȥ
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//��Ͳ����		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//��Ͳץ
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//��Ͳ��ȥ
////	delay_ms(1000);
////	crazyMe(1,50,50,5,80);//צ���ſ�
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//��Ͳ����		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//��Ͳץ
////	delay_ms(1000);
////	crazyMe(2,45,45,5,80);//��Ͳ��ȥ
////	delay_ms(1000);
////	crazyMe(1,50,50,5,80);//צ���ſ�
////	delay_ms(1000);
////	crazyMe(2,93,93,5,80);//��Ͳ����		
////  delay_ms(1000);	
////  crazyMe(1,125,125,5,80);//��Ͳץ

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
////	crazyMe(2,93,93,5,80);//��Ͳ����			
////	delay_ms(1500);
////	crazyMe(1,128,50,5,80);//��Ͳ��	
////	delay_ms(1500);
////	crazyMe(2,46,46,5,80);//��Ͳ��ȥ


////	crazyMe(15,0,45,5,80);		

////		
///*****************��ɫʶ�����************************/
//// while(1)
//// {
////	 OLED_Refresh();
////	 Dis_Temp();
//////	 See_Color();
////	 delay_ms(100);
//// }
///*****************��ʾ���ͻҶȲ���****************/	
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
///*****************�����𲽿�ʼ����***************/	
////	   Go_Head(Begin_ToX);	 											/*�ӳ����㵽C��*/
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
//����Ϊ������ʾ��
//pac9685:IIC3  
//TCS345: IIC1
//��·�Ҷ�ǰ��F
//��·�ҶȺ�G
//����� 1����� ����� 2����ǰ
//����� 3���Һ� ����� 4����ǰ
//L_front   PC1
//L_Centre  PC2
//L_back    PC3
//R_front   PC0
//R_Centre  PF11
//R_back    PB1
//*/

////����һ��ץȡ������C->E->A
//void Task1(void)
//{
//	uint8_t Case,Now_Color;
//	uint8_t DoneClock;
//	DuoJi_Init();
//	Set_want_Position(CC);
//	for(DoneClock=0;DoneClock<3;DoneClock++)
//	{
//		delay_ms(1000);
//		Now_Color=See_Color();									/*ʶ����ɫ*/	
//		if(Color_Flag==0)                       /*Ԥ��û��ʶ����ɫ������ǰ������ٴ�ʶ��*/
//		{
//			TIME_GO(2);
//			delay_ms(1000);		
//			Now_Color=See_Color();								/*ʶ����ɫ*/				
//		}
//		Case=Judge_Case(Now_Color,DoneClock);		/*�ж����*/
//		Task1_Working(Case,Now_Color);					/*�Ե����߲������ִ����Ӧ����*/				
//	}
//}
////���������ʶ��B����ȷ����������ɫ��˳��
//void Task2(void)
//{
//	//�Ȱ���ɫȷ�����ٷֱ�ǰ�����ѽ�����צ����Ӧ��λ��
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

