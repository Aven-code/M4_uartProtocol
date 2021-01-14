#include "stm32f4xx.h"
#include "beep.h"
#include "delay.h"

/*
	�鿴ԭ��ͼ�ã�
	
	BEEP -> PF8
	
	�˷���������Դ��������ֻҪͨ�缴���С�
	BEEP �ź����ӵ� S8050 �����ܵĻ������ߵ�ƽ��ʹ�䵼ͨ���͵�ƽʹ���ֹ��
	����������ӵ�ͨ����������У���ֹ�������������
 */

/**
 * ���ܣ�  ��ʼ��������
 * ������  ��
 * ����ֵ����
 **/
void beep_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// ʹ�� GPIOF ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);
	
	// ���� PF8
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_8;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_DOWN;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	// Ĭ�ϲ�����
	BEEP_OFF();
}

/**
 * ���ܣ�  ���������� count �� 
 * ������  t1 ����ʱ��(ms)
 *         t2 ������ʱ��(ms)
 * ����ֵ����
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

