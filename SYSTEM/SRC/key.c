
#include "stm32f4xx.h"
#include "key.h"
#include "delay.h"

#define KEY_USE_INTERRUPT

/*
	查原理图得：
	
	KEY0 -> PA0
	KEY1 -> PE2
	KEY2 -> PE3
	KEY3 -> PE4
	
	按键是输入设备，MCU 通过 PA0, PE2, PE3, PE4 这四个 IO 口对应的输入数据寄存器来获取输入状态。
	高电平(1)表示未按下，低电平(0)表示按下
	
 */

#ifdef KEY_USE_INTERRUPT
static volatile uint8_t key_value = 0;   // 中断中记录按键按下状态
#endif

/**
 * 功能：  初始化按键
 * 参数：  无
 * 返回值：无
 **/
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
#ifdef KEY_USE_INTERRUPT
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
#endif
	
	// 使能 GPIOA, GPIOE 的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);
	
	// 配置 PA0
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	// GPIO_OType 和 GPIO_Speed 只有在模式为输出/复用时才需要配置，因此此处不需要配置
	//GPIO_InitStruct.GPIO_OType = ;
	//GPIO_InitStruct.GPIO_Speed = ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	// 此处由于外部存在强上拉/下拉，因此可以配置为任意 --> 浮空/上拉/下拉：
	// GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN 三者任意一个都可以
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置 PE2, PE3, PE4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

#ifdef KEY_USE_INTERRUPT
	// 配置外部中断线的 NVIC
	NVIC_InitStruct.NVIC_IRQChannel = EXTI0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI2_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI3_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = EXTI4_IRQn;
	NVIC_Init(&NVIC_InitStruct);
	
	// 使能 SYSCFG 时钟，以配置外部中断线与 GPIO 的连接
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// 使能 EXTI 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);
	
	// void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	// 这里已经使用完了 SYSCFG，因此可以关闭它的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;      // 用于中断
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  // 下降沿触发(按下时触发)
	// void EXTI_Init(EXTI_InitTypeDef* EXTI_InitStruct);
	EXTI_Init(&EXTI_InitStruct);
	
	// ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
	// void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
	EXTI_ClearITPendingBit(EXTI_Line0);
	EXTI_ClearITPendingBit(EXTI_Line2);
	EXTI_ClearITPendingBit(EXTI_Line3);
	EXTI_ClearITPendingBit(EXTI_Line4);
#endif
	
}

/**
 * 功能：  扫描按键 KEY0, KEY1, KEY2, KEY3
 * 参数：  flag 松开标志，1 判断是否松开，0 判断是否按下
 * 返回值：按键 KEY0, KEY1, KEY2, KEY3 的状态：1 - 按下，0 - 未按下
 *         0 位 - KEY0, 1 位 - KEY1, 2 位 - KEY2, 3 位 - KEY3
 **/
#ifndef KEY_USE_INTERRUPT     // 未使用中断的情况

uint8_t key_scan(uint8_t flag)
{
	uint8_t key_state = 0;
	
	if (!KEY0 | !KEY1 | !KEY2 | !KEY3)
	{
		// 前沿延时去抖，延时时间一般在 5ms - 15ms，取决于按键的机械特性
		Delay(5);
		
		if (KEY0 == 0)
		{
			key_state |= KEY_0;
		}
		if (KEY1 == 0)
		{
			key_state |= KEY_1;
		}
		if (KEY2 == 0)
		{
			key_state |= KEY_2;
		}
		if (KEY3 == 0)
		{
			key_state |= KEY_3;
		}
		if (flag & 1)   // 松开标志
		{
			// 直到按键全部松开为止
			while (!KEY0 | !KEY1 | !KEY2 | !KEY3);
			// 后沿延时去抖
			Delay(5);
			// 再次确认所有按键全部松开
			while (!KEY0 | !KEY1 | !KEY2 | !KEY3);
		}
	}
	
	return key_state;
}

#else   // 使用中断的情况

uint8_t key_scan(uint8_t flag)
{
	uint8_t key_state = 0;
	
	// 获取中断中的按键状态
	key_state = key_value;
	// 清除中断中的按键状态
	key_value = 0;
	
	return key_state;
}

#endif

/**
 * 功能：  返回按键状态 key_state 中 key 键是否按下
 * 参数：  key_state 四个按键的状态，应该是调用 key_scan() 的返回结果
 *         key       按键编号：KEY_0, KEY_1, KEY_2, KEY_3
 * 返回值：按下返回 KEY_PRESSED，未按下返回 KEY_NO_PRESSED
 **/
uint8_t key_pressed(uint8_t key_state, uint8_t key)
{
	return ((key_state & 0xF) & (key & 0xF)) != 0;
}

#ifdef KEY_USE_INTERRUPT

// 外部中断线 0 的中断处理程序
void EXTI0_IRQHandler(void)
{
	// ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
	// void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
	// 此处完全可以不作以下判断，但如果是 EXTI[9:5] 或 EXTI[15:10] 上的中断则必须判断
	if (EXTI_GetITStatus(EXTI_Line0))   
	{
		uint32_t count = 168000000 / 5 / 4;
		key_value |= KEY_0;
		while (count--);
	}
	// 清除中断标志
	EXTI_ClearITPendingBit(EXTI_Line0);
}

// 外部中断线 2 的中断处理程序
void EXTI2_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_1;
	while (count--);
	
	// 清除中断标志
	EXTI_ClearITPendingBit(EXTI_Line2);
}

// 外部中断线 3 的中断处理程序
void EXTI3_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_2;
	while (count--);
	
	// 清除中断标志
	EXTI_ClearITPendingBit(EXTI_Line3);
}

// 外部中断线 4 的中断处理程序
void EXTI4_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_3;
	while (count--);
	
	// 清除中断标志
	EXTI_ClearITPendingBit(EXTI_Line4);
}

#endif
