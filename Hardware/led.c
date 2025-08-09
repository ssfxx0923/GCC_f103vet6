#include "led.h" 


//初始化PB4和PA8为输出口.并使能这两个口的时钟		    
//LED IO初始化
void LED_Init(void)
{    	 
  GPIO_InitTypeDef  GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB|RCC_APB2Periph_GPIOC, ENABLE); 

  //GPIOA8初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8;//LED0对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1最高速度为50MHz
  GPIO_Init(GPIOA, &GPIO_InitStructure); // 初始化GPIO
	
  //GPIOB4初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;//LED1对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1最高速度为50MHz
  GPIO_Init(GPIOB, &GPIO_InitStructure); // 初始化GPIO

  //GPIOC1初始化设置
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;//LED2对应IO口
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; // 推挽输出
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; // F1最高速度为50MHz
  GPIO_Init(GPIOC, &GPIO_InitStructure); // 初始化GPIO
	
  GPIO_SetBits(GPIOA, GPIO_Pin_8); // 初始化为高电平，灯灭
  GPIO_SetBits(GPIOB, GPIO_Pin_4); // 初始化为高电平，灯灭
}

void LED_OFF(unsigned char LED )
{
   if(LED == 0)
   {
      GPIO_SetBits(GPIOA, GPIO_Pin_8); // 高电平，灯灭
   }
   else if(LED == 1)
   {
      GPIO_SetBits(GPIOB, GPIO_Pin_4); // 高电平，灯灭
   }
   else if(LED == 2)
   {
      GPIO_ResetBits(GPIOC, GPIO_Pin_1); // 低电平，灯灭
   }
}



void LED_ON(unsigned char LED)
{
   if(LED == 0)
   {
      GPIO_ResetBits(GPIOA, GPIO_Pin_8); // 低电平，灯亮
   }
   else if(LED == 1)
   {
      GPIO_ResetBits(GPIOB, GPIO_Pin_4); // 低电平，灯亮
   }
   else if(LED == 2)
   {
      GPIO_SetBits(GPIOC, GPIO_Pin_1); // 高电平，灯亮
   }
}
