
#include "stm32f4xx.h"
#include "wwdg.h"

static volatile int flag = 0;
static volatile int COUNTER = 0;

/**
 * 功能：  初始化独立看门狗
 * 参数：  WWDG_Prescaler 预分频值。可取值：
 *             WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
 *             WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
 *             WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
 *             WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
 *         WindowValue    窗口值。必须小于 0x80
 *         Counter        计数值。取值范围：0x40 - 0x7F
 * 返回值：无
 **/
void wwdg_init(uint32_t WWDG_Prescaler, uint8_t WindowValue, uint8_t Counter)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	COUNTER = Counter;
	
	// 使能 WWDG 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	// 配置预分频值
	WWDG_SetPrescaler(WWDG_Prescaler);
	
	// 设置窗口值
	WWDG_SetWindowValue(WindowValue);
		
	// 使能看门狗，同时也喂狗
	WWDG_Enable(Counter);
	
	// 配置 NVIC
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	// 清除中断标志
	WWDG_ClearFlag();
	
	// 设置窗口看门狗中断，一旦使能就不能禁用
	WWDG_EnableIT();
	
	COUNTER=Counter;
}

// 设置计数器值，即喂狗	
// void WWDG_SetCounter(uint8_t Counter);

// 获取中断状态
// FlagStatus WWDG_GetFlagStatus(void);

// 清除中断标志
// void WWDG_ClearFlag(void);

/**
 * 功能：  窗口看门狗中断处理程序
 * 参数：  无
 * 返回值：无
 **/
void WWDG_IRQHandler(void)
{
	if (SET == WWDG_GetFlagStatus())
	{
		flag = 1;
		// 中断中喂狗
		WWDG_FEED(COUNTER);
		// 清除中断标志
		WWDG_ClearFlag();
	}
}

/**
 * 功能：  判断是否发生窗口看门狗中断
 * 参数：  无
 * 返回值：有中断返回 1，否则返回 0
 **/
int wwdg_is_int(void)
{
	if (flag)
	{
		flag = 0;
		return 1;
	}
	return flag;
}