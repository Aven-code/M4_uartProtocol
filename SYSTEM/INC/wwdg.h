#ifndef _WWDG_H
#define _WWDG_H

#include "stm32f4xx.h"

// ι��
#define WWDG_FEED(Counter) WWDG_SetCounter(Counter)

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
void wwdg_init(uint32_t WWDG_Prescaler, uint8_t WindowValue, uint8_t Counter);

/**
 * ���ܣ�  �ж��Ƿ������ڿ��Ź��ж�
 * ������  ��
 * ����ֵ�����жϷ��� 1�����򷵻� 0
 **/
int wwdg_is_int(void);

#endif
