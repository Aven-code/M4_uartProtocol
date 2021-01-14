#include "stm32f4xx.h"
#include "tim.h"

/**
 * ���ܣ�  ��ʼ�� TIM2
 * ������  period    �������ڣ����Զ����ؼ���ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         prescaler Ԥ��Ƶֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 * ����ֵ����
 **/ 
void tim2_init(uint16_t period, uint16_t prescaler)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// ʹ�� TIM2 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
	
	// ��ʼ��ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // ���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = period;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	// TIM_TimeBaseInitStruct.TIM_RepetitionCounter = ;  // TIM1, TIM8 ��ʹ��
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);
	
	// ���� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	// ����жϱ�־
	TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	
	// ʹ���ж�
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM2, ENABLE);
}

static volatile int flag = 0;

/**
 * ���ܣ�  TIM2 �жϴ������
 * ������  ��
 * ����ֵ����
 **/
void TIM2_IRQHandler(void)
{
	if (SET == TIM_GetITStatus(TIM2, TIM_IT_Update))
	{
		flag = 1;
		// ����жϱ�־
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
	}
}

/**
 * ���ܣ�  �ж� TIM2 �Ƿ���������ж�
 * ������  ��
 * ����ֵ�������˸����жϷ��� 1�����򷵻� 0
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
	��ԭ��ͼ�ã�

	LED0 -> PF9 / TIM14_CH1

	��˿���ͨ����ʱ�� 14 ��ͨ�� 1 ����� PWM ʵ�ֶ� LED0 �����Ƚ��п��ƣ�ʵ�ֺ����Ƶ�Ч����

 */

/**
 * ���ܣ�  ��ʼ�� TIM14_CH1 �� LED0
 * ������  period    �������ڣ����Զ����ؼ���ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         prescaler Ԥ��Ƶֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         compare   �Ƚ�ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 * ����ֵ����
 **/
void tim14_ch1_led0_init(uint16_t period, uint16_t prescaler, uint16_t compare)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	TIM_OCInitTypeDef TIM_OCInitStruct;
	
	// ʹ�� TIM14 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14, ENABLE);
	
	// ʹ�� GPIOF ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// ���� PF9 ����Ϊ TIM14_CH1
	GPIO_PinAFConfig(GPIOF, GPIO_PinSource9, GPIO_AF_TIM14);
	
	// ���� PF9 
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	// ��ʼ��ʱ����Ԫ
	TIM_TimeBaseInitStruct.TIM_ClockDivision = TIM_CKD_DIV1;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;  // ���ϼ���
	TIM_TimeBaseInitStruct.TIM_Period = period;
	TIM_TimeBaseInitStruct.TIM_Prescaler = prescaler;
	// TIM_TimeBaseInitStruct.TIM_RepetitionCounter = ;  // TIM1, TIM8 ��ʹ��
	TIM_TimeBaseInit(TIM14, &TIM_TimeBaseInitStruct);
	
	// ��������Ƚ�
	// �����ĸ���TIM1 �� TIM8 ר������
	//TIM_OCInitStruct.TIM_OCIdleState = ;
	//TIM_OCInitStruct.TIM_OCNIdleState = ;
	//TIM_OCInitStruct.TIM_OCNPolarity = ;
	//TIM_OCInitStruct.TIM_OutputNState = ;
	TIM_OCInitStruct.TIM_OCMode = TIM_OCMode_PWM1;              // CNT < CCR ʱ OC1REF Ϊ��Ч״̬
	TIM_OCInitStruct.TIM_OCPolarity = TIM_OCPolarity_Low;       // CC1P 1:�͵�ƽ��Ч
	TIM_OCInitStruct.TIM_OutputState = TIM_OutputState_Enable;  // CC1E 1:����ͨ��
	TIM_OCInitStruct.TIM_Pulse = compare;                       // CCR1 �Ƚ�ֵ
	TIM_OC1Init(TIM14, &TIM_OCInitStruct);
	
	// ʹ�ܶ�ʱ��
	TIM_Cmd(TIM14, ENABLE);
	
	// ʹ�� TIM14 Ԥװ�ؼĴ���
	TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable);
	
	// ʹ���Զ�����Ԥװ�ؼĴ���
	TIM_ARRPreloadConfig(TIM14, ENABLE);
	
	// ���� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = TIM8_TRG_COM_TIM14_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
	
	// ����жϱ�־
	TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
	
	// ʹ���ж�
	TIM_ITConfig(TIM14, TIM_IT_Update, ENABLE);
}

static volatile uint32_t led0_pwm_cnt = 0;

/**
 * ���ܣ�  TIM8 ����/�Ƚ� �� TIM14 �жϴ������
 * ������  ��
 * ����ֵ����
 **/
void TIM8_TRG_COM_TIM14_IRQHandler(void)
{
	// ITStatus TIM_GetITStatus(TIM_TypeDef* TIMx, uint16_t TIM_IT);
	// void TIM_ClearITPendingBit(TIM_TypeDef* TIMx, uint16_t TIM_IT);
	// �� TIM14 �ĸ����ж�
	if (TIM_GetITStatus(TIM14, TIM_IT_Update) == SET)
	{
		led0_pwm_cnt++;
		// ����жϱ�־
		TIM_ClearITPendingBit(TIM14, TIM_IT_Update);
	}
}

/**
 * ���ܣ�  �޸� TIM14_CH1 �� PWM ����ֵ
 * ������  cnt PWM ����ֵ
 * ����ֵ����
 **/ 
void tim14_ch1_led0_set_pwm_cnt(uint32_t cnt)
{
	led0_pwm_cnt = cnt;
}

/**
 * ���ܣ�  ��ȡ TIM14_CH1 �� PWM ����ֵ
 * ������  ��
 * ����ֵ����
 **/ 
uint32_t tim14_ch1_led0_get_pwm_cnt(void)
{
	return led0_pwm_cnt;
}
