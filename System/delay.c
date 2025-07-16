#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// ������ֻ��ѧϰʹ�ã�δ��������ɣ��������������κ���;
// ALIENTEK STM32F103������
// ʹ��SysTick����ͨ����ģʽ���ӳٽ��й���
// ����delay_us, delay_ms
// ����ԭ��@ALIENTEK
// ������̳:www.openedv.com
// �޸�����:2024/4/27
// �汾��V1.1
// ��Ȩ���У�����ؾ���
// Copyright(C) ������������ӿƼ����޹�˾ 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

static u8  fac_us = 0; // us��ʱ������
static u16 fac_ms = 0; // ms��ʱ������

// ��ʼ���ӳٺ���
// SYSCLK:ϵͳʱ��Ƶ��(MHz)
void delay_init(u8 SYSCLK)
{
    SysTick->CTRL = 0; // �ر�SysTick
    SysTick->LOAD = 0; // �������ֵ
    SysTick->VAL  = 0; // ��յ�ǰ����ֵ
    fac_us = SYSCLK;   // 1us��Ҫ�ļ�����
    fac_ms = (u16)fac_us * 1000; // 1ms��Ҫ�ļ�����
}

// ��ʱnus΢��
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us; // ��������ֵ
    SysTick->VAL = 0x00;          // ��ռ�����
    SysTick->CTRL = SysTick_CTRL_ENABLE; // ����SysTick
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // �ȴ�������0
    SysTick->CTRL = 0; // �ر�SysTick
    SysTick->VAL = 0;  // ��ռ�����
}

// ��ʱnms����
void delay_ms(u32 nms)
{
    while (nms--)
    {
        delay_us(1000);
    }
}

// ��ʱnms,���������������
void delay_xms(u32 nms)
{
    delay_ms(nms);
}

// ��ʱns��
void delay_s(u32 ns)
{
    while (ns--)
    {
        delay_ms(1000);
    }
}

			 



































