#ifndef __IIC_H
#define __IIC_H 			   
#include <sys.h>	  
	   		   
//IO��������
#define IIC_SDA_IN()  {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x08000000;}	//PC14����ģʽ
#define IIC_SDA_OUT() {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x03000000;} //PC14���ģʽ
//IO��������	 
#define IIC_SCL    PCout(13) //SCLʹ��PC13
#define IIC_SDA    PCout(14) //SDAʹ��PC14
#define READ_SDA    PCin(14)  //����SDA 

//IIC���в�������
void IIC_Init(void);                //��ʼ��IIC��IO��				 
void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  			//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);			//IIC����һ���ֽ�
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�
u8 IIC_Wait_Ack(void); 				//IIC�ȴ�ACK�ź�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź�  

#endif



