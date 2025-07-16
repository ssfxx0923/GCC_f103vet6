#include "sys.h"  
#include "stm32f10x.h"                  // Device header

//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F103VE开发板
//系统时钟初始化	
//包括时钟设置/中断管理/GPIO设置等
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


//THUMB指令不支持汇编内联
//采用如下方法实现执行汇编指令WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//关闭所有中断(但是不包括fault和NMI中断)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//开启所有中断
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//设置栈顶地址
//addr:栈顶地址
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
//////////////////////////////////////////////////////////////////////////////////	 
//本程序只供学习使用，未经作者许可，不得用于其它任何用途
//ALIENTEK STM32F407开发板
//系统时钟初始化	
//包括时钟设置/中断管理/GPIO设置等
//正点原子@ALIENTEK
//技术论坛:www.openedv.com
//创建日期:2014/5/2
//版本：V1.1
//版权所有，盗版必究。
//Copyright(C) 广州市星翼电子科技有限公司 2014-2024
//All rights reserved
//********************************************************************************
//修改说明
//V1.1 20150411 
//1,修改WFI_SET/INTX_DISABLE/INTX_ENABLE等函数的实现方式 
//2,修改SYSTEM_SUPPORT_UCOS为SYSTEM_SUPPORT_OS
//////////////////////////////////////////////////////////////////////////////////  


//设置向量表偏移地址
//NVIC_VectTab:基址
//Offset:偏移量		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//设置NVIC的向量表偏移寄存器,VTOR低9位保留,即[8:0]保留。
}
//设置NVIC分组
//NVIC_Group:NVIC分组 0~4 总共5组 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//取后三位
	temp1<<=8;
	temp=SCB->AIRCR;  //读取先前的设置
	temp&=0X0000F8FF; //清空先前分组
	temp|=0X05FA0000; //写入钥匙
	temp|=temp1;	   
	SCB->AIRCR=temp;  //设置分组	    	  				   
}
//设置NVIC 
//NVIC_PreemptionPriority:抢占优先级
//NVIC_SubPriority       :响应优先级
//NVIC_Channel           :中断编号
//NVIC_Group             :中断分组 0~4
//注意优先级不能超过设定的组的范围!否则会有意想不到的错误
//组划分:
//组0:0位抢占优先级,4位响应优先级
//组1:1位抢占优先级,3位响应优先级
//组2:2位抢占优先级,2位响应优先级
//组3:3位抢占优先级,1位响应优先级
//组4:4位抢占优先级,0位响应优先级
//NVIC_SubPriority和NVIC_PreemptionPriority的原则是,数值越小,越优先	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//设置分组
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//取低四位
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//使能中断位(要清除的话,设置ICER对应位为1即可)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//设置响应优先级和抢断优先级   	    	  				   
} 
//外部中断配置函数
//只针对GPIOA~G;不包括PVD,RTC等
//参数:
//GPIOx:0~6,代表GPIOA~G
//BITx:需要使能的位;
//TRIM:触发模式,1,下升沿;2,上降沿;3，任意电平触发
//该函数一次只能配置1个IO口,多个IO口,需多次调用
//该函数会自动开启对应中断,以及屏蔽线   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR = BITx / 4;//得到中断寄存器组的编号
	EXTOFFSET = (BITx % 4) * 4; 
	RCC->APB2ENR |= 0x01;//开启AFIO时钟
	AFIO->EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET);//清除原来设置
	AFIO->EXTICR[EXTADDR] |= GPIOx << EXTOFFSET;//EXTI.BITx映射到GPIOx.BITx 
	//自动设置
	EXTI->IMR |= 1 << BITx;//开启line BITx上的中断(如果要禁止中断，则反操作即可)
	if(TRIM & 0x01)EXTI->FTSR |= 1 << BITx;//line BITx上事件下降沿触发
	if(TRIM & 0x02)EXTI->RTSR |= 1 << BITx;//line BITx上事件上升降沿触发
} 	
//GPIO复用设置
//GPIOx: GPIOA~G
//GPIO_Pins: GPIO引脚（多个引脚可用|组合）
//GPIO_Mode: 复用模式
//注意：该函数不同于F4的GPIO_AF_Set，仅对部分复用功能有效
void GPIO_AF_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pins, u8 GPIO_Mode)
{
    // 对于STM32F1，复用功能通过AFIO->MAPR配置
    // 这里简化处理，具体外设重映射需使用标准库的GPIO_PinRemapConfig
    // 或直接操作AFIO->MAPR寄存器
    
    RCC->APB2ENR |= 0x01; // 使能AFIO时钟
    
    // 此函数为基础实现，具体外设重映射应根据需求单独配置
    // 如需配置USART1重映射: AFIO->MAPR |= 1<<2;
    // 如需配置I2C1重映射: AFIO->MAPR |= 1<<1;
    // 如需配置SPI1重映射: AFIO->MAPR |= 1<<0;
}   
//GPIO通用设置 
//GPIOx:GPIOA~G
//GPIO_Pin:要设置的GPIO_Pin值,如GPIO_Pin_5 | GPIO_Pin_7
//MODE:  0,输入模式(系统复位默认状态)
//       1,通用推挽输出模式
//       2,通用开漏输出模式
//       3,复用推挽输出模式
//       4,复用开漏输出模式
//       5,模拟输入模式
//       6,浮空输入
//       7,上拉/下拉输入
//SPEED: 0,2MHz(低速)
//       1,10MHz(中速)
//       2,50MHz(快速)
void GPIO_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, u8 MODE, u8 SPEED)
{
    u32 pinpos = 0, pos = 0, curpin = 0;
    RCC->APB2ENR |= 1 << (((u32)GPIOx - GPIOA_BASE) / 0x400 + 2);//使能GPIO时钟
    
    for(pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;               // 一个个位检查
        curpin = GPIO_Pin & pos;         // 检查引脚是否要设置
        if(curpin == pos)                // 需要设置
        {
            if(pinpos < 8)
            {
                GPIOx->CRL &= ~(0x0F << (pinpos * 4)); // 清除旧设置
                if(MODE <= 4)            // 输出模式
                {   
                    GPIOx->CRL |= ((u32)((MODE & 3) << 2) | SPEED) << (pinpos * 4);
                }
                else                     // 输入模式
                {
                    if(MODE == 5)        // 模拟输入
                    {
                        GPIOx->CRL |= 0 << (pinpos * 4);
                    }
                    else if(MODE == 6)   // 浮空输入
                    {
                        GPIOx->CRL |= 4 << (pinpos * 4);
                    }
                    else                  // 上/下拉输入
                    {
                        GPIOx->CRL |= 8 << (pinpos * 4);
                        if(MODE == 7)     // 上拉输入
                            GPIOx->ODR |= pos;
                        else              // 下拉输入
                            GPIOx->ODR &= ~pos;
                    }
                }
            }
            else
            {
                pinpos -= 8;
                GPIOx->CRH &= ~(0x0F << (pinpos * 4)); // 清除旧设置
                if(MODE <= 4)            // 输出模式
                {   
                    GPIOx->CRH |= ((u32)((MODE & 3) << 2) | SPEED) << (pinpos * 4);
                }
                else                     // 输入模式
                {
                    if(MODE == 5)        // 模拟输入
                    {
                        GPIOx->CRH |= 0 << (pinpos * 4);
                    }
                    else if(MODE == 6)   // 浮空输入
                    {
                        GPIOx->CRH |= 4 << (pinpos * 4);
                    }
                    else                  // 上/下拉输入
                    {
                        GPIOx->CRH |= 8 << (pinpos * 4);
                        if(MODE == 7)     // 上拉输入
                            GPIOx->ODR |= (1 << (pinpos+8));
                        else              // 下拉输入
                            GPIOx->ODR &= ~(1 << (pinpos+8));
                    }
                }
            }
        }
    }
} 

////THUMB指令不支持汇编内联
////采用如下方法实现执行汇编指令WFI  
//void WFI_SET(void)
//{
//	__ASM volatile("wfi");		  
//}
////关闭所有中断(但是不包括fault和NMI中断)
//void INTX_DISABLE(void)
//{
//	__ASM volatile("cpsid i");
//}
////开启所有中断
//void INTX_ENABLE(void)
//{
//	__ASM volatile("cpsie i");		  
//}
////设置栈顶地址
////addr:栈顶地址
//__asm void MSR_MSP(u32 addr) 
//{
//	MSR MSP, r0 			//set Main Stack value
//	BX r14
//}
//进入待机模式	  
void Sys_Standby(void)
{ 
	SCB->SCR|=1<<2;		//使能SLEEPDEEP位 (SYS->CTRL)	   
	RCC->APB1ENR|=1<<28;//使能电源时钟 
	PWR->CSR|=1<<8;     //设置WKUP用于唤醒
	PWR->CR|=1<<2;      //清除Wake-up 标志
	PWR->CR|=1<<1;      //PDDS置位   	
	WFI_SET();			//执行WFI指令,进入待机模式		 
}	     
//系统软复位   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 		 
//系统时钟初始化函数
//PLL:倍频系数(2~16)，实际倍频PLL+2
//72M:PLL=7,外部晶振8M
//96M:PLL=10,外部晶振8M
//108M:PLL=11,外部晶振8M
//120M:PLL=13,外部晶振8M
//144M:PLL=15,外部晶振8M
//168M:CANNOT,外部晶振8M
void Stm32_Clock_Init(u8 PLL)
{
    unsigned char temp = 0;
    MY_NVIC_PriorityGroupConfig(2); // 设置中断优先级分组2
    RCC->CR |= 0x00010000;          // 外部高速时钟使能HSEON
    while(!(RCC->CR >> 17));        // 等待外部时钟就绪
    RCC->CFGR = 0x00000400;         // APB1=DIV2;APB2=DIV1;AHB=DIV1;
    PLL -= 2;                       // 减2是因为PLL倍频实际为PLL+2
    RCC->CFGR |= PLL << 18;         // 设置PLL值 2~16
    RCC->CFGR |= 1 << 16;           // PLLSRC ON 
    FLASH->ACR |= 0x32;             // FLASH 2个延时周期
    RCC->CR |= 0x01000000;          // PLLON
    while(!(RCC->CR >> 25));        // 等待PLL锁定
    RCC->CFGR |= 0x00000002;        // PLL作为系统时钟	 
    while(temp != 0x02)             // 等待PLL作为系统时钟设置成功
    {   
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
    
    // 配置向量表
#ifdef VECT_TAB_RAM
    MY_NVIC_SetVectorTable(0x20000000, 0x0);
#else
    MY_NVIC_SetVectorTable(0x08000000, 0x0);
#endif
}		   
