#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ�� DAC1
 * ������  ��
 * ����ֵ����
 **/
void dac1_init(void);

/**
 * ���ܣ�  �� DAC1 �������
 * ������  data Ҫ����������ź�����
 * ����ֵ����
 **/
void dac1_output_data(uint16_t data);

#endif
	