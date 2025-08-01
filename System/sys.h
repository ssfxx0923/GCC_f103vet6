#ifndef __SYS_H
#define __SYS_H	 
#include "stm32f10x.h" 
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103VE开发板
//系统时钟初始化	
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//修改日期:2024/5/2
//版本：V1.5
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.5 20240502
//适配STM32F103VET6
////////////////////////////////////////////////////////////////////////////////// 


//0,不支持os
//1,支持os
#define SYSTEM_SUPPORT_OS		0		//定义系统文件夹是否支持OS
															    
//位带操作,实现51类似的GPIO控制功能
//具体实现思想,参考<<CM3权威指南>>第五章(87页~92页).
//IO口操作宏定义
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 
//IO口地址映射
#define GPIOA_ODR_Addr    (GPIOA_BASE+12) //0x4001080C 
#define GPIOB_ODR_Addr    (GPIOB_BASE+12) //0x40010C0C 
#define GPIOC_ODR_Addr    (GPIOC_BASE+12) //0x4001100C 
#define GPIOD_ODR_Addr    (GPIOD_BASE+12) //0x4001140C 
#define GPIOE_ODR_Addr    (GPIOE_BASE+12) //0x4001180C 
#define GPIOF_ODR_Addr    (GPIOF_BASE+12) //0x40011A0C    
#define GPIOG_ODR_Addr    (GPIOG_BASE+12) //0x40011E0C    

#define GPIOA_IDR_Addr    (GPIOA_BASE+8) //0x40010808 
#define GPIOB_IDR_Addr    (GPIOB_BASE+8) //0x40010C08 
#define GPIOC_IDR_Addr    (GPIOC_BASE+8) //0x40011008 
#define GPIOD_IDR_Addr    (GPIOD_BASE+8) //0x40011408 
#define GPIOE_IDR_Addr    (GPIOE_BASE+8) //0x40011808 
#define GPIOF_IDR_Addr    (GPIOF_BASE+8) //0x40011A08 
#define GPIOG_IDR_Addr    (GPIOG_BASE+8) //0x40011E08 
 
//IO口操作,只对单一的IO口!
//确保n的值小于16!
#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //输出 
#define PAin(n)    BIT_ADDR(GPIOA_IDR_Addr,n)  //输入 

#define PBout(n)   BIT_ADDR(GPIOB_ODR_Addr,n)  //输出 
#define PBin(n)    BIT_ADDR(GPIOB_IDR_Addr,n)  //输入 

#define PCout(n)   BIT_ADDR(GPIOC_ODR_Addr,n)  //输出 
#define PCin(n)    BIT_ADDR(GPIOC_IDR_Addr,n)  //输入 

#define PDout(n)   BIT_ADDR(GPIOD_ODR_Addr,n)  //输出 
#define PDin(n)    BIT_ADDR(GPIOD_IDR_Addr,n)  //输入 

#define PEout(n)   BIT_ADDR(GPIOE_ODR_Addr,n)  //输出 
#define PEin(n)    BIT_ADDR(GPIOE_IDR_Addr,n)  //输入

#define PFout(n)   BIT_ADDR(GPIOF_ODR_Addr,n)  //输出 
#define PFin(n)    BIT_ADDR(GPIOF_IDR_Addr,n)  //输入

#define PGout(n)   BIT_ADDR(GPIOG_ODR_Addr,n)  //输出 
#define PGin(n)    BIT_ADDR(GPIOG_IDR_Addr,n)  //输入

// STM32F103系列F1芯片特有类型定义
typedef uint32_t  u32;
typedef uint16_t u16;
typedef uint8_t  u8;

// 函数声明
void Stm32_Clock_Init(u8 PLL);  // 系统时钟初始化函数(72M)
void WFI_SET(void);             // 执行WFI指令
void INTX_DISABLE(void);        // 关闭所有中断
void INTX_ENABLE(void);         // 开启所有中断
void MSR_MSP(u32 addr);         // 设置堆栈地址

// 以下为常用函数
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab, u32 Offset);                      // 设置中断向量表位置
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group);                                // 设置中断分组
void MY_NVIC_Init(u8 NVIC_PreemptionPriority, u8 NVIC_SubPriority, u8 NVIC_Channel, u8 NVIC_Group); // 中断初始化
void Ex_NVIC_Config(u8 GPIOx, u8 BITx, u8 TRIM);                               // 外部中断配置函数
void GPIO_AF_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pins, u8 GPIO_Mode);            // GPIO复用设置 




void GPIO_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, u8 MODE, u8 SPEED);           // GPIO设置

void Sys_Soft_Reset(void);      // 系统软复位
void Sys_Standby(void);         // 进入待机模式

#endif











