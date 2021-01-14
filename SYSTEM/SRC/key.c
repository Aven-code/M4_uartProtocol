
#include "stm32f4xx.h"
#include "key.h"
#include "delay.h"

#define KEY_USE_INTERRUPT

/*
	��ԭ��ͼ�ã�
	
	KEY0 -> PA0
	KEY1 -> PE2
	KEY2 -> PE3
	KEY3 -> PE4
	
	�����������豸��MCU ͨ�� PA0, PE2, PE3, PE4 ���ĸ� IO �ڶ�Ӧ���������ݼĴ�������ȡ����״̬��
	�ߵ�ƽ(1)��ʾδ���£��͵�ƽ(0)��ʾ����
	
 */

#ifdef KEY_USE_INTERRUPT
static volatile uint8_t key_value = 0;   // �ж��м�¼��������״̬
#endif

/**
 * ���ܣ�  ��ʼ������
 * ������  ��
 * ����ֵ����
 **/
void key_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
#ifdef KEY_USE_INTERRUPT
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
#endif
	
	// ʹ�� GPIOA, GPIOE ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOE, ENABLE);
	
	// ���� PA0
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_IN;
	// GPIO_OType �� GPIO_Speed ֻ����ģʽΪ���/����ʱ����Ҫ���ã���˴˴�����Ҫ����
	//GPIO_InitStruct.GPIO_OType = ;
	//GPIO_InitStruct.GPIO_Speed = ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_0;
	// �˴������ⲿ����ǿ����/��������˿�������Ϊ���� --> ����/����/������
	// GPIO_PuPd_NOPULL, GPIO_PuPd_UP, GPIO_PuPd_DOWN ��������һ��������
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// ���� PE2, PE3, PE4
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3 | GPIO_Pin_4;
	GPIO_Init(GPIOE, &GPIO_InitStruct);

#ifdef KEY_USE_INTERRUPT
	// �����ⲿ�ж��ߵ� NVIC
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
	
	// ʹ�� SYSCFG ʱ�ӣ��������ⲿ�ж����� GPIO ������
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
	// ʹ�� EXTI ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_EXTIT, ENABLE);
	
	// void SYSCFG_EXTILineConfig(uint8_t EXTI_PortSourceGPIOx, uint8_t EXTI_PinSourcex)
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource2);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource3);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOE, EXTI_PinSource4);
	// �����Ѿ�ʹ������ SYSCFG����˿��Թر�����ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, DISABLE);
	
	EXTI_InitStruct.EXTI_Line = EXTI_Line0 | EXTI_Line2 | EXTI_Line3 | EXTI_Line4;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;      // �����ж�
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Falling;  // �½��ش���(����ʱ����)
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
 * ���ܣ�  ɨ�谴�� KEY0, KEY1, KEY2, KEY3
 * ������  flag �ɿ���־��1 �ж��Ƿ��ɿ���0 �ж��Ƿ���
 * ����ֵ������ KEY0, KEY1, KEY2, KEY3 ��״̬��1 - ���£�0 - δ����
 *         0 λ - KEY0, 1 λ - KEY1, 2 λ - KEY2, 3 λ - KEY3
 **/
#ifndef KEY_USE_INTERRUPT     // δʹ���жϵ����

uint8_t key_scan(uint8_t flag)
{
	uint8_t key_state = 0;
	
	if (!KEY0 | !KEY1 | !KEY2 | !KEY3)
	{
		// ǰ����ʱȥ������ʱʱ��һ���� 5ms - 15ms��ȡ���ڰ����Ļ�е����
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
		if (flag & 1)   // �ɿ���־
		{
			// ֱ������ȫ���ɿ�Ϊֹ
			while (!KEY0 | !KEY1 | !KEY2 | !KEY3);
			// ������ʱȥ��
			Delay(5);
			// �ٴ�ȷ�����а���ȫ���ɿ�
			while (!KEY0 | !KEY1 | !KEY2 | !KEY3);
		}
	}
	
	return key_state;
}

#else   // ʹ���жϵ����

uint8_t key_scan(uint8_t flag)
{
	uint8_t key_state = 0;
	
	// ��ȡ�ж��еİ���״̬
	key_state = key_value;
	// ����ж��еİ���״̬
	key_value = 0;
	
	return key_state;
}

#endif

/**
 * ���ܣ�  ���ذ���״̬ key_state �� key ���Ƿ���
 * ������  key_state �ĸ�������״̬��Ӧ���ǵ��� key_scan() �ķ��ؽ��
 *         key       ������ţ�KEY_0, KEY_1, KEY_2, KEY_3
 * ����ֵ�����·��� KEY_PRESSED��δ���·��� KEY_NO_PRESSED
 **/
uint8_t key_pressed(uint8_t key_state, uint8_t key)
{
	return ((key_state & 0xF) & (key & 0xF)) != 0;
}

#ifdef KEY_USE_INTERRUPT

// �ⲿ�ж��� 0 ���жϴ������
void EXTI0_IRQHandler(void)
{
	// ITStatus EXTI_GetITStatus(uint32_t EXTI_Line);
	// void EXTI_ClearITPendingBit(uint32_t EXTI_Line);
	// �˴���ȫ���Բ��������жϣ�������� EXTI[9:5] �� EXTI[15:10] �ϵ��ж�������ж�
	if (EXTI_GetITStatus(EXTI_Line0))   
	{
		uint32_t count = 168000000 / 5 / 4;
		key_value |= KEY_0;
		while (count--);
	}
	// ����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line0);
}

// �ⲿ�ж��� 2 ���жϴ������
void EXTI2_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_1;
	while (count--);
	
	// ����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line2);
}

// �ⲿ�ж��� 3 ���жϴ������
void EXTI3_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_2;
	while (count--);
	
	// ����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line3);
}

// �ⲿ�ж��� 4 ���жϴ������
void EXTI4_IRQHandler(void)
{
	uint32_t count = 168000000 / 5 / 4;
	key_value |= KEY_3;
	while (count--);
	
	// ����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line4);
}

#endif
