#include "led.h" 


//��ʼ��PB4��PA8Ϊ�����.��ʹ���������ڵ�ʱ��		    
//LED IO��ʼ��
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 

  //GPIOA8��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//LED0��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1����ٶ�Ϊ50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure); // ��ʼ��GPIO
	
  //GPIOB4��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//LED1��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1����ٶ�Ϊ50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure); // ��ʼ��GPIO

  //GPIOC1��ʼ������
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//LED2��ӦIO��
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // �������
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1����ٶ�Ϊ50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure); // ��ʼ��GPIO
	
  GPIO_SetBits(GPIOA, GPIO_Pin_8); // ��ʼ��Ϊ�ߵ�ƽ������
  GPIO_SetBits(GPIOB, GPIO_Pin_4); // ��ʼ��Ϊ�ߵ�ƽ������
}

void LED_OFF(unsigned char LED )
{
   if(LED == 0)
   {
      GPIO_SetBits(GPIOA, GPIO_Pin_8); // �ߵ�ƽ������
   }
   else if(LED == 1)
   {
      GPIO_SetBits(GPIOB, GPIO_Pin_4); // �ߵ�ƽ������
   }
   else if(LED == 2)
   {
      GPIO_ResetBits(GPIOC, GPIO_Pin_1); // �͵�ƽ������
   }
}



void LED_ON(unsigned char LED)
{
   if(LED == 0)
   {
      GPIO_ResetBits(GPIOA, GPIO_Pin_8); // �͵�ƽ������
   }
   else if(LED == 1)
   {
      GPIO_ResetBits(GPIOB, GPIO_Pin_4); // �͵�ƽ������
   }
   else if(LED == 2)
   {
      GPIO_SetBits(GPIOC, GPIO_Pin_1); // �ߵ�ƽ������
   }
}
