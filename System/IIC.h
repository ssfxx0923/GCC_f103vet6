#ifndef __IIC1_2_h__
#define __IIC1_2h__		   
#include <sys.h>	  
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经许可不得用于其它任何用途
//ALIENTEK STM32F103开发板
//IIC 驱动代码	   
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2014/5/6
//版本：V1.0
//版权所有，盗版必究
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO方向设置
#define IIC_SDA_IN()  {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x08000000;}	//PE14输入模式
#define IIC_SDA_OUT() {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x03000000;} //PE14输出模式
//IO操作函数	 
#define IIC_SCL    PCout(13) //SCL使用PE13
#define IIC_SDA    PCout(14) //SDA使用PE14
#define READ_SDA    PCin(14)  //输入SDA 

//IIC所有操作函数
void IIC_Init(void);                //初始化IIC的IO口				 
void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  			//发送IIC停止信号
void IIC_Send_Byte(u8 txd);			//IIC发送一个字节
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节
u8 IIC_Wait_Ack(void); 				//IIC等待ACK信号
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif




