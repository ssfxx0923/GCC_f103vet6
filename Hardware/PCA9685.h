#ifndef __PCA9685_H
#define __PCA9685_H

#include "sys.h"
#include "IIC.h"

    //PCA9685�豸��ַ
#define PCA9685_adrr (0x40<<1)

//��ؼĴ�����ַ
#define PCA9685_SUBADR1 0x2
#define PCA9685_SUBADR2 0x3
#define PCA9685_SUBADR3 0x4

//MODE1�Ĵ�����ַ
#define PCA9685_MODE1 0x0

//PRE_SCALE�Ĵ�����ַ
#define PCA9685_PRESCALE 0xFE

//0ͨ����ON��OFF�Ĵ�����ַ
#define LED0_ON_L 0x6
#define LED0_ON_H 0x7
#define LED0_OFF_L 0x8
#define LED0_OFF_H 0x9

#define ALLLED_ON_L 0xFA
#define ALLLED_ON_H 0xFB
#define ALLLED_OFF_L 0xFC
#define ALLLED_OFF_H 0xFD

/* Exported types ------------------------------------------------------------*/
// ����˶������ṹ��
typedef struct {
    u8 channel;        // ���ͨ���� (0-15)
    u16 start_angle;   // ��ʼ�Ƕ� (0-180)
    u16 target_angle;  // Ŀ��Ƕ� (0-180)
    u8 enable;         // �Ƿ����ô˶�� (0=������, 1=����)
} ServoMotion;

/* Exported functions --------------------------------------------------------*/					
void PCA9685_Init();
void PCA9685_write(unsigned char reg,unsigned char data);
u8 PCA9685_read(unsigned char reg);
void setPWMFreq(u8 freq);
void setPWM(u8 num, u16 on, u16 off);
u16 calculate_PWM(u8 angle);
void crazyMe(int i,u16 a,u16 b,u16 tim,u8 xf);
void crazyMe_Multi(ServoMotion* servos, u8 servo_count, u16 step_time, u8 total_steps);

#endif







