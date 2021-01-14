#include "stm32f4xx.h"
#include "beep.h"
#include "delay.h"

/*
	查看原理图得：
	
	BEEP -> PF8
	
	此蜂鸣器是有源蜂鸣器，只要通电即鸣叫。
	BEEP 信号连接到 S8050 三极管的基极，高电平则使其导通，低电平使其截止，
	因此三极管子导通则蜂鸣器鸣叫，截止则蜂鸣器不鸣叫
 */

/**
 * 功能：  初始化蜂鸣器
 * 参数：  无
 * 返回值：无
 **/
void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// 使能 GPIOF 的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// 配置 PF8
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	// 默认不鸣叫
	BEEP_OFF();
}

/**
 * 功能：  蜂鸣器鸣叫 count 次 
 * 参数：  t1 鸣叫时长(ms)
 *         t2 不鸣叫时长(ms)
 * 返回值：无
 **/
void beeps(int count, int t1, int t2)
{
	while (count-- > 0)
	{
		BEEP_ON();
		Delay(t1 / 10);
		BEEP_OFF();
		Delay(t2 / 10);
	}
}

