/**
  ******************************************************************************
  * @file    	PCA9685.c
  * @author  	MC_Wang
  * @version 	V1.0
  * @date    	2021��3��27��
  * @brief   	STM32ͨ��ģ��IICЭ����PCA9685ͨѶ
 @verbatim
 ===============================================================================
                        ##### How to use this driver #####
 ===============================================================================
    [..]
      (#)ʹ��setPWMFreq(u8 freq)������ʼ��PCA9685;
			(++)u8 freq:���ö����PWMƵ�ʣ�����ʹ��u8����(0~255)
			��PCA9685�����������ԼΪ1.6KHz��Ƶ�ʣ����Ҫ���ø���Ƶ�ʣ���Ҫ���β����ͺ�PCA9685_write(unsigned char reg,unsigned char data)
			����Ȥ�����Լ����ԣ����������u8���ͣ����50Hz����freq = 50;����
	  (#)ʹ��PCA9685_write(unsigned char reg,unsigned char data)��ѡ�е�PCA9685��XX�Ĵ���дXX����
			(++)unsigned char reg��PCA9685�Ĵ�����ַ
			(++)unsigned char data:��ѡ�е�PCA9685��ĳ�Ĵ���д�������
	  (#)ʹ��u8 PCA9685_read(unsigned char reg)��ѡ�е�PCA9685��XX�Ĵ�����ȡXX����
			(++)unsigned char reg��PCA9685�Ĵ�����ַ
			(++)����ֵΪPCA9685ĳ�Ĵ���������
	  (#)����PCA9685ĳͨ����ռ�ձȣ���ʹ��setPWM(u8 num, u16 on, u16 off)
	  (#)������ĽǶ�ת����LEDx_OFF��ֵ����ʹ��calculate_PWM(u8 angle)
 @endverbatim
  *
  ******************************************************************************
  * @attention
  * <h2><center>&copy; COPYRIGHT 2021 JXUST@WangMingcong</center></h2>
	******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "PCA9685.h"
#include "IIC.h"
#include "delay.h"
#include "math.h"
#include "Usart1.h"
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/** @defgroup PCA9685_Private_Functions
  * @{
  */
/** @defgroup PCA9685 Initialization and Configuration
 *  @brief   Initialization and Configuration
 *
 @verbatim
 ===============================================================================
                 ##### PCA9685��ʼ��������PCA9685��PWMƵ�� #####
 ===============================================================================

 @endverbatim
  * @{
  */

void PCA9685_Init()
{
  IIC_Init();
  PCA9685_write(PCA9685_MODE1, 0x0); // �ȸ�λPCA9685
  delay_ms(1); // �ȴ���λ���
  setPWMFreq(50); // Ȼ������PCA9685Ƶ��Ϊ50HZ	
}

/**
  * @brief  ��ѡ�е�PCA9685��XX�Ĵ���дXX����
  * @note   ��ѡ��PCA9685��STM32��ͨ��IIC��PCA9685дPCA9685�豸��ַ,��PCA9685_adrr��0x80
						��ͨ���βν�����д�뵽PCA9685�Ĵ�����
  * @param  unsigned char reg
  *            @arg �Ĵ�����ַ
  * @param  unsigned char data
  *            @arg ��Ĵ���д�������
  * @date   2021��3��27��
  * @retval None
  */
 void PCA9685_write(unsigned char reg,unsigned char data)//IICЭ��
{	
    IIC_Start();				 //������ʼ�ź�
    IIC_Send_Byte(PCA9685_adrr);//����PCA9685�豸��ַ
    IIC_Wait_Ack();				//�ȴ�Ӧ��
    IIC_Send_Byte(reg);			//��������
    IIC_Wait_Ack();
    IIC_Send_Byte(data);
    IIC_Wait_Ack();
    IIC_Stop();					//����ֹͣ�ź�
}
/**
  * @brief  ��ѡ�е�PCA9685��XX�Ĵ�����ȡXX����
  * @note   ��ѡ��PCA9685��STM32��ͨ��IIC��PCA9685дPCA9685�豸��ַ,��PCA9685_adrr��0x80
						��ͨ���βζ�ȡPCA9685�Ĵ����е�����
  * @param  unsigned char reg
  *            @arg PCA9685�Ĵ�����ַ
  * @date   2021��3��27��
  * @retval  ��ȡ���Ĵ����е�����
  */
u8 PCA9685_read(unsigned char reg)
{
    u8 res;
    IIC_Start();
    IIC_Send_Byte(PCA9685_adrr); // ����д��ַ
    IIC_Wait_Ack();
    IIC_Send_Byte(reg);
    IIC_Wait_Ack();
    IIC_Start();
    IIC_Send_Byte(PCA9685_adrr | 0x01); // �޸�Ϊֱ����Ӷ�λ����������
    IIC_Wait_Ack();
    res=IIC_Read_Byte(0);
    IIC_Stop();
    return res;
}

/**
  * @brief  ��ʼ��PCA9685������PWM���Ƶ��
  * @note   1.����PCA9685��MODE1�Ĵ�����ֵ:oldmode == 0x33 = 0011 0011(����λ��ʹ���ڲ�25MHzʱ�ӡ��ڲ���ַ��д����Զ����ӡ�����sleep����Ӧ0x70ͨ��IIC��ַ)
						2.����λPCA9685�ҽ���sleepģʽ
						3.����PCA9685��PRE_SCALE�Ĵ���������PCA9685�����PWMƵ��
						4.�˳�sleepģʽ
  * @param  u8 freq
  *            @arg  ����PCA9685�����PWMƵ�ʣ�0~255��Hz��Ҳ����PPT�е�update_rate
  * @date   2021��3��27��
  * @retval None
  */
void setPWMFreq(u8 freq)
{
    u8 prescale,oldmode,newmode;
    double prescaleval;
    prescaleval = 25000000.0/(4096*freq*0.915);
    prescale = (u8)floor(prescaleval+0.5)-1;//floor()������ȡ������

    oldmode = PCA9685_read(PCA9685_MODE1);
//����ʹ�ã�
//		printf("%d\r\n",oldmode);//oldmode == 0x33 = 0011 0011
    newmode = (oldmode&0x7F) | 0x10; // ����λ�ҽ���sleepģʽ
    PCA9685_write(PCA9685_MODE1, newmode); // go to sleep
    PCA9685_write(PCA9685_PRESCALE, prescale); // set the prescaler
    PCA9685_write(PCA9685_MODE1, oldmode);
    delay_ms(5);
    // ��ȷ�����˳�˯��ģʽ�������Զ�����
    PCA9685_write(PCA9685_MODE1, oldmode | 0x20); // ����AUTO_INCREMENTλ(0x20)
}


/**
  * @brief  ���ø���ͨ����ON��OFF�Ĵ������Ӷ��ﵽ�ı�PCA9685���PWM��ռ�ձ�
  * @note   ��ΪPCA9685��PWM��12λ�ģ����Դ���Ĳ���Ϊu16��16λ�ſ���װ��12λ�����ݣ�
						��16λ�����У�����λû�ã�on��off��ֵ��0~4096 ����
  * @param  u8 num      
  *            @arg	ͨ���� (0~15)                  
  * @param  u16 on
  *            @arg �Ƶ�LEDX_ONʱ����Ϊ�ߵ�ƽ
  * @param  u16 off
  *            @arg ������LEDX_OFFʱ����Ϊ�͵�ƽ
  * @date  	2021��3��27��
  * @retval None
  */
void setPWM(u8 num, u16 on, u16 off)
{
    PCA9685_write(LED0_ON_L+4*num,on);			//�Ĵ�����ַ,��Ĵ���д�������
    PCA9685_write(LED0_ON_H+4*num,on>>8);
    PCA9685_write(LED0_OFF_L+4*num,off);
    PCA9685_write(LED0_OFF_H+4*num,off>>8);
}

/**
  * @brief  ������ĽǶȼ����12λLEDX_OFF��ֵ
  * @author MC_Wang
  * @note   ͨ�����㹫ʽ��������ĽǶȼ����12λLEDX_OFF��ֵ
���㹫ʽ��PWM = 4096*(0.5 + (2.5-0.5)*(u8 angle)/180)/20

  * @param  u8 angle       
  *            @arg ����Ƕ�                    
  * @date  	2021��3��27��
  * @retval 12λLEDX_OFF��ֵ
  */
u16 calculate_PWM(u8 angle)
{
    // �������㷽����ʹ����ע���еĹ�ʽһ��
    return (u16)(4096*(0.5 + (2.5-0.5)*angle/180.0)/20.0);
}

/************************ (C) COPYRIGHT JXUST@WangMingcong *****END OF FILE****/
void crazyMe(int i, u16 a, u16 b, u16 tim, u8 xf) // i������λ a��Ŀǰ�Ƕ� b��Ŀ��Ƕ� tim��ÿһ����ʱ�� xf�������ָ�����
{
    u16 k, m;
    u8 s;
    for(s=0; s<=xf; s++)
    {
        if(a>b)
        {
            k = calculate_PWM(a) - calculate_PWM(b);
        }
        else
        {
            k = calculate_PWM(b) - calculate_PWM(a); // ɾ������ֺ�
        }
        
        if(a>b)
        {
            m = calculate_PWM(a) - k*s/xf;
        }
        else
        {
            m = calculate_PWM(a) + k*s/xf; // �Ľ��������﷨
        }
        setPWM(i, 0, m);
        delay_ms(tim);
    }
}

/**
  * @brief  ����ͬʱ�˶����ƺ���
  * @note   ͨ���ṹ��������ƶ�����ͬʱ�˶���֧��ѡ��������/����
  *         �������õĶ����ͬʱ��ʼ�˶���ͬʱ�����˶�
  * @param  ServoMotion* servos
  *            @arg ����˶���������ָ��
  * @param  u8 servo_count
  *            @arg �����ж��������
  * @param  u16 step_time
  *            @arg ÿ������ʱʱ��(ms)
  * @param  u8 total_steps
  *            @arg ���˶�����������Խ���˶�Խƽ��
  * @date   2025��
  * @retval None
  */
void crazyMe_Multi(ServoMotion* servos, u8 servo_count, u16 step_time, u8 total_steps)
{
    u8 step, i;
    u16 current_pwm;
    s16 angle_diff;  // ʹ���з���������ǶȲ�
    
    // ��ÿ��ʱ�䲽�����ѭ��
    for(step = 0; step <= total_steps; step++)
    {
        // ͬʱ�����������õĶ��
        for(i = 0; i < servo_count; i++)
        {
            if(servos[i].enable)  // ֻ�������õĶ��
            {
                // ����ǶȲ�����ţ�֧���������˶���
                angle_diff = (s16)servos[i].target_angle - (s16)servos[i].start_angle;
                
                // ���㵱ǰ����Ӧ�õ���ĽǶ�
                u16 current_angle = servos[i].start_angle + (angle_diff * step) / total_steps;
                
                // ת��ΪPWMֵ�����õ���Ӧͨ��
                current_pwm = calculate_PWM(current_angle);
                setPWM(servos[i].channel, 0, current_pwm);
            }
        }
        
        // ���ж��ͬ����ʱ��ȷ���˶�Э��
        delay_ms(step_time);
    }
}

