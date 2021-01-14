#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"

#define DELAY_USE_INT    // 使用中断延时

void Delay(__IO uint32_t nTime);

#ifndef DELAY_USE_INT

/**
 * 功能：  SysTick 定时器初始化
 * 参数：  systick_clksrc SysTick 时钟源选择：
 *                        SysTick_CLKSource_HCLK     : AHB, 
 *                        SysTick_CLKSource_HCLK_Div8: AHB/8
 * 返回值：无
 **/
void systick_init(uint32_t systick_clksrc);
	
/**
 * 功能：  延时 nus 微秒
 * 参数：  nus 微秒数
 * 返回值：无
 **/
void delay_us(uint32_t nus);

/**
 * 功能：  延时 nms 微秒
 * 参数：  nms 毫秒数
 * 返回值：无
 **/
void delay_ms(uint32_t nms);

#endif

#endif
