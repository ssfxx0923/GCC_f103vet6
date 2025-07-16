#ifndef __IIC_H
#define __IIC_H 			   
#include <sys.h>	  
	   		   
//IO方向设置
#define IIC_SDA_IN()  {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x08000000;}	//PC14输入模式
#define IIC_SDA_OUT() {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x03000000;} //PC14输出模式
//IO操作函数	 
#define IIC_SCL    PCout(13) //SCL使用PC13
#define IIC_SDA    PCout(14) //SDA使用PC14
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

#endif



