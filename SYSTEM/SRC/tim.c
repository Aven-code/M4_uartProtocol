#include "stm32f4xx.h"
#include "tim.h"

/**
 * 功能：  初始化 TIM2
 * 参数：  period    计数周期，即自动重载计数值。取值范围：0x0000 - 0xFFFF
 *         prescaler 预分频值。取值范围：0x0000 - 0xFFFF
 * 返回值：无
 **/ 
void tim2_init(uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 使能 TIM2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// 初始化时基单元
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
	TIM_TimeBaseInitStruct.TIM_Period = period;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	// TIM_TimeBaseInitStruct.TIM_RepetitionCounter = ;  // TIM1, TIM8 才使用
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	// 配置 NVIC
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	// 清除中断标志
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	// 使能中断
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// 使能定时器
	TIM_Cmd(TIM2, ENABLE);
}

static volatile int flag = 0;

/**
 * 功能：  TIM2 中断处理程序
 * 参数：  无
 * 返回值：无
 **/
void TIM2_IRQHandler(void)
{
	if (SET == TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		flag = 1;
		// 清除中断标志
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

/**
 * 功能：  判断 TIM2 是否产生更新中断
 * 参数：  无
 * 返回值：产生了更新中断返回 1，否则返回 0
 **/
int tim2_is_int(void)
{
	if (flag)
	{
		flag = 0;
		return 1;
	}
	return flag;
}

/*
	查原理图得：

	LED0 -> PF9 / TIM14_CH1

	因此可以通过定时器 14 的通道 1 来输出 PWM 实现对 LED0 的亮度进行控制，实现呼吸灯的效果。

 */

/**
 * 功能：  初始化 TIM14_CH1 和 LED0
 * 参数：  period    计数周期，即自动重载计数值。取值范围：0x0000 - 0xFFFF
 *         prescaler 预分频值。取值范围：0x0000 - 0xFFFF
 *         compare   比较值。取值范围：0x0000 - 0xFFFF
 * 返回值：无
 **/
void tim14_ch1_led0_init(uint16_t period, uint16_t prescaler, uint16_t compare)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	// 使能 TIM14 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	// 使能 GPIOF 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// 配置 PF9 复用为 TIM14_CH1
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	// 配置 PF9 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	// 初始化时基单元
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // 向上计数
	TIM_TimeBaseInitStruct.TIM_Period = period;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	// TIM_TimeBaseInitStruct.TIM_RepetitionCounter = ;  // TIM1, TIM8 才使用
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
	
	// 配置输出比较
	// 以下四个是TIM1 和 TIM8 专用配置
	//TIM_OCInitStruct.TIM_OCIdleState = ;
	//TIM_OCInitStruct.TIM_OCNIdleState = ;
	//TIM_OCInitStruct.TIM_OCNPolarity = ;
	//TIM_OCInitStruct.TIM_OutputNState = ;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;              // CNT < CCR 时 OC1REF 为有效状态
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;       // CC1P 1:低电平有效
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // CC1E 1:开启通道
	TIM_OCInitStruct.TIM_Pulse = compare;                       // CCR1 比较值
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	// 使能定时器
	TIM_Cmd(TIM14, ENABLE);
	
	// 使能 TIM14 预装载寄存器
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
	
	// 使能自动重载预装载寄存器
	TIM_ARRPreloadConfig(TIM14, ENABLE);
	
	// 配置 NVIC
	NVIC_InitStruct.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	// 清除中断标志
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
	
	// 使能中断
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
}

static volatile uint32_t led0_pwm_cnt = 0;

/**
 * 功能：  TIM8 触发/比较 和 TIM14 中断处理程序
 * 参数：  无
 * 返回值：无
 **/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	// ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
	// void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);
	// 是 TIM14 的更新中断
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET)
	{
		led0_pwm_cnt++;
		// 清除中断标志
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
	}
}

/**
 * 功能：  修改 TIM14_CH1 的 PWM 计数值
 * 参数：  cnt PWM 计数值
 * 返回值：无
 **/ 
void tim14_ch1_led0_set_pwm_cnt(uint32_t cnt)
{
	led0_pwm_cnt = cnt;
}

/**
 * 功能：  获取 TIM14_CH1 的 PWM 计数值
 * 参数：  无
 * 返回值：无
 **/ 
uint32_t tim14_ch1_led0_get_pwm_cnt(void)
{
	return led0_pwm_cnt;
}
