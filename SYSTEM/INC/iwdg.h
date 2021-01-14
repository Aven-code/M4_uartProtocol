#ifndef _IWDG_H
#define _IWDG_H

#include "stm32f4xx.h"

// ι��
#define IWDG_FEED IWDG_ReloadCounter

/**
 * ���ܣ�  ��ʼ���������Ź�
 * ������  IWDG_Prescaler Ԥ��Ƶֵ���� LSI �Ļ����Ͻ��з�Ƶ����ȡֵ��
 *             IWDG_Prescaler_4
 *             IWDG_Prescaler_8
 *             IWDG_Prescaler_16
 *             IWDG_Prescaler_32
 *             IWDG_Prescaler_64
 *             IWDG_Prescaler_128
 *             IWDG_Prescaler_256
 *         Reload         ��װ��ֵ��ȡֵ��ΧΪ 0x0000 - 0x0FFF
 * ����ֵ����
 **/
void iwdg_init(uint8_t IWDG_Prescaler, uint16_t Reload);

#endif
