
#include "stm32f4xx.h"
#include "wwdg.h"

static volatile int flag = 0;
static volatile int COUNTER = 0;

/**
 * ���ܣ�  ��ʼ���������Ź�
 * ������  WWDG_Prescaler Ԥ��Ƶֵ����ȡֵ��
 *             WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
 *             WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
 *             WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
 *             WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
 *         WindowValue    ����ֵ������С�� 0x80
 *         Counter        ����ֵ��ȡֵ��Χ��0x40 - 0x7F
 * ����ֵ����
 **/
void wwdg_init(uint32_t WWDG_Prescaler, uint8_t WindowValue, uint8_t Counter)
{
	NVIC_InitTypeDef NVIC_InitStruct;
	
	COUNTER = Counter;
	
	// ʹ�� WWDG ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);
	
	// ����Ԥ��Ƶֵ
	WWDG_SetPrescaler(WWDG_Prescaler);
	
	// ���ô���ֵ
	WWDG_SetWindowValue(WindowValue);
		
	// ʹ�ܿ��Ź���ͬʱҲι��
	WWDG_Enable(Counter);
	
	// ���� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = WWDG_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 1;
	NVIC_Init(&NVIC_InitStruct);
	
	// ����жϱ�־
	WWDG_ClearFlag();
	
	// ���ô��ڿ��Ź��жϣ�һ��ʹ�ܾͲ��ܽ���
	WWDG_EnableIT();
	
	COUNTER=Counter;
}

// ���ü�����ֵ����ι��	
// void WWDG_SetCounter(uint8_t Counter);

// ��ȡ�ж�״̬
// FlagStatus WWDG_GetFlagStatus(void);

// ����жϱ�־
// void WWDG_ClearFlag(void);

/**
 * ���ܣ�  ���ڿ��Ź��жϴ������
 * ������  ��
 * ����ֵ����
 **/
void WWDG_IRQHandler(void)
{
	if (SET == WWDG_GetFlagStatus())
	{
		flag = 1;
		// �ж���ι��
		WWDG_FEED(COUNTER);
		// ����жϱ�־
		WWDG_ClearFlag();
	}
}

/**
 * ���ܣ�  �ж��Ƿ������ڿ��Ź��ж�
 * ������  ��
 * ����ֵ�����жϷ��� 1�����򷵻� 0
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