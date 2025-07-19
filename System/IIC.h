#ifndef __IIC1_2_h__
#define __IIC1_2h__		   
#include <sys.h>	  
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ����ɲ������������κ���;
//ALIENTEK STM32F103������
//IIC ��������	   
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2014/5/6
//�汾��V1.0
//��Ȩ���У�����ؾ�
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved									  
////////////////////////////////////////////////////////////////////////////////// 	
   	   		   
//IO��������
#define IIC_SDA_IN()  {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x08000000;}	//PE14����ģʽ
#define IIC_SDA_OUT() {GPIOC->CRH&=0xF0FFFFFF;GPIOC->CRH|=0x03000000;} //PE14���ģʽ
//IO��������	 
#define IIC_SCL    PCout(13) //SCLʹ��PE13
#define IIC_SDA    PCout(14) //SDAʹ��PE14
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

void IIC_Write_One_Byte(u8 daddr,u8 addr,u8 data);
u8 IIC_Read_One_Byte(u8 daddr,u8 addr);	  
#endif




