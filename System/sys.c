#include "sys.h"  
#include "stm32f10x.h"                  // Device header

//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F103VE������
//ϵͳʱ�ӳ�ʼ��	
//����ʱ������/�жϹ���/GPIO���õ�
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//�޸�����:2024/5/2
//�汾��V1.5
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.5 20240502
//����STM32F103VET6
//////////////////////////////////////////////////////////////////////////////////  


//THUMBָ�֧�ֻ������
//�������·���ʵ��ִ�л��ָ��WFI  
__asm void WFI_SET(void)
{
	WFI;		  
}
//�ر������ж�(���ǲ�����fault��NMI�ж�)
__asm void INTX_DISABLE(void)
{
	CPSID   I
	BX      LR	  
}
//���������ж�
__asm void INTX_ENABLE(void)
{
	CPSIE   I
	BX      LR  
}
//����ջ����ַ
//addr:ջ����ַ
__asm void MSR_MSP(u32 addr) 
{
	MSR MSP, r0 			//set Main Stack value
	BX r14
}
//////////////////////////////////////////////////////////////////////////////////	 
//������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
//ALIENTEK STM32F407������
//ϵͳʱ�ӳ�ʼ��	
//����ʱ������/�жϹ���/GPIO���õ�
//����ԭ��@ALIENTEK
//������̳:www.openedv.com
//��������:2014/5/2
//�汾��V1.1
//��Ȩ���У�����ؾ���
//Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
//All rights reserved
//********************************************************************************
//�޸�˵��
//V1.1 20150411 
//1,�޸�WFI_SET/INTX_DISABLE/INTX_ENABLE�Ⱥ�����ʵ�ַ�ʽ 
//2,�޸�SYSTEM_SUPPORT_UCOSΪSYSTEM_SUPPORT_OS
//////////////////////////////////////////////////////////////////////////////////  


//����������ƫ�Ƶ�ַ
//NVIC_VectTab:��ַ
//Offset:ƫ����		 
void MY_NVIC_SetVectorTable(u32 NVIC_VectTab,u32 Offset)	 
{ 	   	  
	SCB->VTOR=NVIC_VectTab|(Offset&(u32)0xFFFFFE00);//����NVIC��������ƫ�ƼĴ���,VTOR��9λ����,��[8:0]������
}
//����NVIC����
//NVIC_Group:NVIC���� 0~4 �ܹ�5�� 		   
void MY_NVIC_PriorityGroupConfig(u8 NVIC_Group)	 
{ 
	u32 temp,temp1;	  
	temp1=(~NVIC_Group)&0x07;//ȡ����λ
	temp1<<=8;
	temp=SCB->AIRCR;  //��ȡ��ǰ������
	temp&=0X0000F8FF; //�����ǰ����
	temp|=0X05FA0000; //д��Կ��
	temp|=temp1;	   
	SCB->AIRCR=temp;  //���÷���	    	  				   
}
//����NVIC 
//NVIC_PreemptionPriority:��ռ���ȼ�
//NVIC_SubPriority       :��Ӧ���ȼ�
//NVIC_Channel           :�жϱ��
//NVIC_Group             :�жϷ��� 0~4
//ע�����ȼ����ܳ����趨����ķ�Χ!����������벻���Ĵ���
//�黮��:
//��0:0λ��ռ���ȼ�,4λ��Ӧ���ȼ�
//��1:1λ��ռ���ȼ�,3λ��Ӧ���ȼ�
//��2:2λ��ռ���ȼ�,2λ��Ӧ���ȼ�
//��3:3λ��ռ���ȼ�,1λ��Ӧ���ȼ�
//��4:4λ��ռ���ȼ�,0λ��Ӧ���ȼ�
//NVIC_SubPriority��NVIC_PreemptionPriority��ԭ����,��ֵԽС,Խ����	   
void MY_NVIC_Init(u8 NVIC_PreemptionPriority,u8 NVIC_SubPriority,u8 NVIC_Channel,u8 NVIC_Group)	 
{ 
	u32 temp;	  
	MY_NVIC_PriorityGroupConfig(NVIC_Group);//���÷���
	temp=NVIC_PreemptionPriority<<(4-NVIC_Group);	  
	temp|=NVIC_SubPriority&(0x0f>>NVIC_Group);
	temp&=0xf;								//ȡ����λ
	NVIC->ISER[NVIC_Channel/32]|=1<<NVIC_Channel%32;//ʹ���ж�λ(Ҫ����Ļ�,����ICER��ӦλΪ1����)
	NVIC->IP[NVIC_Channel]|=temp<<4;				//������Ӧ���ȼ����������ȼ�   	    	  				   
} 
//�ⲿ�ж����ú���
//ֻ���GPIOA~G;������PVD,RTC��
//����:
//GPIOx:0~6,����GPIOA~G
//BITx:��Ҫʹ�ܵ�λ;
//TRIM:����ģʽ,1,������;2,�Ͻ���;3�������ƽ����
//�ú���һ��ֻ������1��IO��,���IO��,���ε���
//�ú������Զ�������Ӧ�ж�,�Լ�������   	    
void Ex_NVIC_Config(u8 GPIOx,u8 BITx,u8 TRIM) 
{ 
	u8 EXTADDR;
	u8 EXTOFFSET;
	EXTADDR = BITx / 4;//�õ��жϼĴ�����ı��
	EXTOFFSET = (BITx % 4) * 4; 
	RCC->APB2ENR |= 0x01;//����AFIOʱ��
	AFIO->EXTICR[EXTADDR] &= ~(0x000F << EXTOFFSET);//���ԭ������
	AFIO->EXTICR[EXTADDR] |= GPIOx << EXTOFFSET;//EXTI.BITxӳ�䵽GPIOx.BITx 
	//�Զ�����
	EXTI->IMR |= 1 << BITx;//����line BITx�ϵ��ж�(���Ҫ��ֹ�жϣ��򷴲�������)
	if(TRIM & 0x01)EXTI->FTSR |= 1 << BITx;//line BITx���¼��½��ش���
	if(TRIM & 0x02)EXTI->RTSR |= 1 << BITx;//line BITx���¼��������ش���
} 	
//GPIO��������
//GPIOx: GPIOA~G
//GPIO_Pins: GPIO���ţ�������ſ���|��ϣ�
//GPIO_Mode: ����ģʽ
//ע�⣺�ú�����ͬ��F4��GPIO_AF_Set�����Բ��ָ��ù�����Ч
void GPIO_AF_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pins, u8 GPIO_Mode)
{
    // ����STM32F1�����ù���ͨ��AFIO->MAPR����
    // ����򻯴�������������ӳ����ʹ�ñ�׼���GPIO_PinRemapConfig
    // ��ֱ�Ӳ���AFIO->MAPR�Ĵ���
    
    RCC->APB2ENR |= 0x01; // ʹ��AFIOʱ��
    
    // �˺���Ϊ����ʵ�֣�����������ӳ��Ӧ�������󵥶�����
    // ��������USART1��ӳ��: AFIO->MAPR |= 1<<2;
    // ��������I2C1��ӳ��: AFIO->MAPR |= 1<<1;
    // ��������SPI1��ӳ��: AFIO->MAPR |= 1<<0;
}   
//GPIOͨ������ 
//GPIOx:GPIOA~G
//GPIO_Pin:Ҫ���õ�GPIO_Pinֵ,��GPIO_Pin_5 | GPIO_Pin_7
//MODE:  0,����ģʽ(ϵͳ��λĬ��״̬)
//       1,ͨ���������ģʽ
//       2,ͨ�ÿ�©���ģʽ
//       3,�����������ģʽ
//       4,���ÿ�©���ģʽ
//       5,ģ������ģʽ
//       6,��������
//       7,����/��������
//SPEED: 0,2MHz(����)
//       1,10MHz(����)
//       2,50MHz(����)
void GPIO_Set(GPIO_TypeDef* GPIOx, u16 GPIO_Pin, u8 MODE, u8 SPEED)
{
    u32 pinpos = 0, pos = 0, curpin = 0;
    RCC->APB2ENR |= 1 << (((u32)GPIOx - GPIOA_BASE) / 0x400 + 2);//ʹ��GPIOʱ��
    
    for(pinpos = 0; pinpos < 16; pinpos++)
    {
        pos = 1 << pinpos;               // һ����λ���
        curpin = GPIO_Pin & pos;         // ��������Ƿ�Ҫ����
        if(curpin == pos)                // ��Ҫ����
        {
            if(pinpos < 8)
            {
                GPIOx->CRL &= ~(0x0F << (pinpos * 4)); // ���������
                if(MODE <= 4)            // ���ģʽ
                {   
                    GPIOx->CRL |= ((u32)((MODE & 3) << 2) | SPEED) << (pinpos * 4);
                }
                else                     // ����ģʽ
                {
                    if(MODE == 5)        // ģ������
                    {
                        GPIOx->CRL |= 0 << (pinpos * 4);
                    }
                    else if(MODE == 6)   // ��������
                    {
                        GPIOx->CRL |= 4 << (pinpos * 4);
                    }
                    else                  // ��/��������
                    {
                        GPIOx->CRL |= 8 << (pinpos * 4);
                        if(MODE == 7)     // ��������
                            GPIOx->ODR |= pos;
                        else              // ��������
                            GPIOx->ODR &= ~pos;
                    }
                }
            }
            else
            {
                pinpos -= 8;
                GPIOx->CRH &= ~(0x0F << (pinpos * 4)); // ���������
                if(MODE <= 4)            // ���ģʽ
                {   
                    GPIOx->CRH |= ((u32)((MODE & 3) << 2) | SPEED) << (pinpos * 4);
                }
                else                     // ����ģʽ
                {
                    if(MODE == 5)        // ģ������
                    {
                        GPIOx->CRH |= 0 << (pinpos * 4);
                    }
                    else if(MODE == 6)   // ��������
                    {
                        GPIOx->CRH |= 4 << (pinpos * 4);
                    }
                    else                  // ��/��������
                    {
                        GPIOx->CRH |= 8 << (pinpos * 4);
                        if(MODE == 7)     // ��������
                            GPIOx->ODR |= (1 << (pinpos+8));
                        else              // ��������
                            GPIOx->ODR &= ~(1 << (pinpos+8));
                    }
                }
            }
        }
    }
} 

////THUMBָ�֧�ֻ������
////�������·���ʵ��ִ�л��ָ��WFI  
//void WFI_SET(void)
//{
//	__ASM volatile("wfi");		  
//}
////�ر������ж�(���ǲ�����fault��NMI�ж�)
//void INTX_DISABLE(void)
//{
//	__ASM volatile("cpsid i");
//}
////���������ж�
//void INTX_ENABLE(void)
//{
//	__ASM volatile("cpsie i");		  
//}
////����ջ����ַ
////addr:ջ����ַ
//__asm void MSR_MSP(u32 addr) 
//{
//	MSR MSP, r0 			//set Main Stack value
//	BX r14
//}
//�������ģʽ	  
void Sys_Standby(void)
{ 
	SCB->SCR|=1<<2;		//ʹ��SLEEPDEEPλ (SYS->CTRL)	   
	RCC->APB1ENR|=1<<28;//ʹ�ܵ�Դʱ�� 
	PWR->CSR|=1<<8;     //����WKUP���ڻ���
	PWR->CR|=1<<2;      //���Wake-up ��־
	PWR->CR|=1<<1;      //PDDS��λ   	
	WFI_SET();			//ִ��WFIָ��,�������ģʽ		 
}	     
//ϵͳ��λ   
void Sys_Soft_Reset(void)
{   
	SCB->AIRCR =0X05FA0000|(u32)0x04;	  
} 		 
//ϵͳʱ�ӳ�ʼ������
//PLL:��Ƶϵ��(2~16)��ʵ�ʱ�ƵPLL+2
//72M:PLL=7,�ⲿ����8M
//96M:PLL=10,�ⲿ����8M
//108M:PLL=11,�ⲿ����8M
//120M:PLL=13,�ⲿ����8M
//144M:PLL=15,�ⲿ����8M
//168M:CANNOT,�ⲿ����8M
void Stm32_Clock_Init(u8 PLL)
{
    unsigned char temp = 0;
    MY_NVIC_PriorityGroupConfig(2); // �����ж����ȼ�����2
    RCC->CR |= 0x00010000;          // �ⲿ����ʱ��ʹ��HSEON
    while(!(RCC->CR >> 17));        // �ȴ��ⲿʱ�Ӿ���
    RCC->CFGR = 0x00000400;         // APB1=DIV2;APB2=DIV1;AHB=DIV1;
    PLL -= 2;                       // ��2����ΪPLL��Ƶʵ��ΪPLL+2
    RCC->CFGR |= PLL << 18;         // ����PLLֵ 2~16
    RCC->CFGR |= 1 << 16;           // PLLSRC ON 
    FLASH->ACR |= 0x32;             // FLASH 2����ʱ����
    RCC->CR |= 0x01000000;          // PLLON
    while(!(RCC->CR >> 25));        // �ȴ�PLL����
    RCC->CFGR |= 0x00000002;        // PLL��Ϊϵͳʱ��	 
    while(temp != 0x02)             // �ȴ�PLL��Ϊϵͳʱ�����óɹ�
    {   
        temp = RCC->CFGR >> 2;
        temp &= 0x03;
    }
    
    // ����������
#ifdef VECT_TAB_RAM
    MY_NVIC_SetVectorTable(0x20000000, 0x0);
#else
    MY_NVIC_SetVectorTable(0x08000000, 0x0);
#endif
}		   
