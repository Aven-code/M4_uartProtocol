#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"

// �������
#define KEY_0 (1 << 0)
#define KEY_1 (1 << 1)
#define KEY_2 (1 << 2)
#define KEY_3 (1 << 3)

// ��������/δ���±�־
#define KEY_NO_PRESSED 0
#define KEY_PRESSED    1

#if 0

// ʹ��λ����ȡ
#define KEY0 PAin(0)
#define KEY1 PEin(2)
#define KEY2 PEin(3)
#define KEY3 PEin(4)

#else

// ʹ�ÿ⺯����ȡ
// uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#define KEY0 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define KEY2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
#define KEY3 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)

#endif  // 0

/**
 * ���ܣ�  ��ʼ������
 * ������  ��
 * ����ֵ����
 **/
void key_init(void);

/**
 * ���ܣ�  ɨ�谴�� KEY0, KEY1, KEY2, KEY3
 * ������  flag �ɿ���־��1 �ж��Ƿ��ɿ���0 �ж��Ƿ���
 * ����ֵ������ KEY0, KEY1, KEY2, KEY3 ��״̬��1 - ���£�0 - δ����
 *         0 λ - KEY0, 1 λ - KEY1, 2 λ - KEY2, 3 λ - KEY3
 **/
uint8_t key_scan(uint8_t flag);

/**
 * ���ܣ�  ���ذ���״̬ key_state �� key ���Ƿ���
 * ������  key_state �ĸ�������״̬��Ӧ���ǵ��� key_scan() �ķ��ؽ��
 *         key       ������ţ�KEY_0, KEY_1, KEY_2, KEY_3
 * ����ֵ�����·��� KEY_PRESSED��δ���·��� KEY_NO_PRESSED
 **/
uint8_t key_pressed(uint8_t key_state, uint8_t key);

#endif  // _KEY_H
