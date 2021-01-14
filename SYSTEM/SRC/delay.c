
#include "delay.h"

static __IO uint32_t uwTimingDelay;

#ifndef DELAY_USE_INT

__IO uint32_t fac_us = 0;   // us ��ʱ����

/**
 * ���ܣ�  SysTick ��ʱ����ʼ��
 * ������  systick_clksrc SysTick ʱ��Դѡ��
 *                        SysTick_CLKSource_HCLK     : AHB, 
 *                        SysTick_CLKSource_HCLK_Div8: AHB/8
 * ����ֵ���� 
 **/
void systick_init(uint32_t systick_clksrc)
{
	RCC_ClocksTypeDef RCC_Clocks;
	
	RCC_GetClocksFreq(&RCC_Clocks);
	SysTick->LOAD  = 0UL;
  SysTick->VAL   = 0UL;
	if (systick_clksrc == SysTick_CLKSource_HCLK)
	{
		SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk;   // ѡ�� AHB ��Ϊʱ��Դ
		fac_us = RCC_Clocks.HCLK_Frequency / 1000000;  // �õ� us ��ʱ����
	}
	else
	{
		SysTick->CTRL  = 0;   // ѡ�� AHB / 8 ��Ϊʱ��Դ
		fac_us = RCC_Clocks.HCLK_Frequency / 8000000;  // �õ� us ��ʱ����
	}
}

/**
 * ���ܣ�  �ڲ�ʹ�õ���ʱ nus ����
 * ������  nus ΢����
 * ����ֵ����
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
 * ���ܣ�  ��ʱ nus ΢��
 * ������  nus ΢����
 * ����ֵ����
 **/
void delay_us(uint32_t nus)
{
	uint32_t us = (1 << 24) / fac_us;  // ÿ������ʱ�����΢����
	
	while (nus >= us)
	{
		_delay_us(us);
		nus -= us;
	}
	_delay_us(nus);
}

/**
 * ���ܣ�  ��ʱ nms ΢��
 * ������  nms ������
 * ����ֵ����
 **/
void delay_ms(uint32_t nms)
{
	uint32_t ms = (1 << 24) / fac_us / 1000;  // ÿ������ʱ����������
	while (nms >= ms)
	{
		_delay_us(ms * 1000);
		nms -= ms;
	}
	_delay_us(nms * 1000);
}

// 10ms Ϊ��λ��ʱ
void Delay(__IO uint32_t nTime)
{
	delay_ms(nTime * 10);
}

#else    // ʹ���ж���ʱ

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
