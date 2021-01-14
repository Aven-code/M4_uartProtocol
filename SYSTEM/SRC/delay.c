
#include "delay.h"

static __IO uint32_t uwTimingDelay;

#ifndef DELAY_USE_INT

__IO uint32_t fac_us = 0;   // us 延时因子

/**
 * 功能：  SysTick 定时器初始化
 * 参数：  systick_clksrc SysTick 时钟源选择：
 *                        SysTick_CLKSource_HCLK     : AHB, 
 *                        SysTick_CLKSource_HCLK_Div8: AHB/8
 * 返回值：无 
 **/
void systick_init(uint32_t systick_clksrc)
{
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick->LOAD  = 0UL;
  SysTick->VAL   = 0UL;
	if (systick_clksrc == SysTick_CLKSource_HCLK)
	{
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk;   // 选择 AHB 作为时钟源
		fac_us = RCC_Clocks.HCLK_Frequency / 1000000;  // 得到 us 延时因子
	}
	else
	{
		SysTick->CTRL  = 0;   // 选择 AHB / 8 作为时钟源
		fac_us = RCC_Clocks.HCLK_Frequency / 8000000;  // 得到 us 延时因子
	}
}

/**
 * 功能：  内部使用的延时 nus 函数
 * 参数：  nus 微秒数
 * 返回值：无
 **/
static void _delay_us(uint32_t nus)
{
	SysTick->LOAD = nus * fac_us - 1;
  SysTick->VAL  = 0UL;
	SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;
	while ((SysTick->CTRL & SysTick_CTRL_COUNTFLAG_Msk) != SysTick_CTRL_COUNTFLAG_Msk);
	SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
}

/**
 * 功能：  延时 nus 微秒
 * 参数：  nus 微秒数
 * 返回值：无
 **/
void delay_us(uint32_t nus)
{
	uint32_t us = (1 << 24) / fac_us;  // 每次能延时的最大微秒数
	
	while (nus >= us)
	{
		_delay_us(us);
		nus -= us;
	}
	_delay_us(nus);
}

/**
 * 功能：  延时 nms 微秒
 * 参数：  nms 毫秒数
 * 返回值：无
 **/
void delay_ms(uint32_t nms)
{
	uint32_t ms = (1 << 24) / fac_us / 1000;  // 每次能延时的最大毫秒数
	while (nms >= ms)
	{
		_delay_us(ms * 1000);
		nms -= ms;
	}
	_delay_us(nms * 1000);
}

// 10ms 为单位延时
void Delay(__IO uint32_t nTime)
{
	delay_ms(nTime * 10);
}

#else    // 使用中断延时

/**
  * @brief  Inserts a delay time.
  * @param  nTime: specifies the delay time length, in milliseconds.
  * @retval None
  */
void Delay(__IO uint32_t nTime)
{ 
  uwTimingDelay = nTime;

  while(uwTimingDelay != 0);
}

#endif

/**
  * @brief  Decrements the TimingDelay variable.
  * @param  None
  * @retval None
  */
void TimingDelay_Decrement(void)
{
  if (uwTimingDelay != 0x00)
  { 
    uwTimingDelay--;
  }
}
