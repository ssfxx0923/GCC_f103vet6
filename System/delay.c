#include "delay.h"
#include "sys.h"
//////////////////////////////////////////////////////////////////////////////////
// 本程序只供学习使用，未经作者许可，不得用于其它任何用途
// ALIENTEK STM32F103开发板
// 使用SysTick的普通计数模式对延迟进行管理
// 包含delay_us, delay_ms
// 正点原子@ALIENTEK
// 技术论坛:www.openedv.com
// 修改日期:2024/4/27
// 版本：V1.1
// 版权所有，盗版必究。
// Copyright(C) 广州市星翼电子科技有限公司 2014-2024
// All rights reserved
//////////////////////////////////////////////////////////////////////////////////

static u8  fac_us = 0; // us延时倍乘数
static u16 fac_ms = 0; // ms延时倍乘数

// 初始化延迟函数
// SYSCLK:系统时钟频率(MHz)
void delay_init(u8 SYSCLK)
{
    SysTick->CTRL = 0; // 关闭SysTick
    SysTick->LOAD = 0; // 清空重载值
    SysTick->VAL  = 0; // 清空当前计数值
    fac_us = SYSCLK;   // 1us需要的计数数
    fac_ms = (u16)fac_us * 1000; // 1ms需要的计数数
}

// 延时nus微秒
void delay_us(u32 nus)
{
    u32 temp;
    SysTick->LOAD = nus * fac_us; // 设置重载值
    SysTick->VAL = 0x00;          // 清空计数器
    SysTick->CTRL = SysTick_CTRL_ENABLE; // 启动SysTick
    do
    {
        temp = SysTick->CTRL;
    } while ((temp & 0x01) && !(temp & (1 << 16))); // 等待计数到0
    SysTick->CTRL = 0; // 关闭SysTick
    SysTick->VAL = 0;  // 清空计数器
}

// 延时nms毫秒
void delay_ms(u32 nms)
{
    while (nms--)
    {
        delay_us(1000);
    }
}

// 延时nms,不会引起任务调度
void delay_xms(u32 nms)
{
    delay_ms(nms);
}

// 延时ns秒
void delay_s(u32 ns)
{
    while (ns--)
    {
        delay_ms(1000);
    }
}

			 



































