#ifndef _ADC_H
#define _ADC_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ�� ADC1_IN5
 * ������  prescaler ģ���·ʱ��Ԥ��Ƶֵ����ȡֵ��
 *             ADC_Prescaler_Div2
 *             ADC_Prescaler_Div4
 *             ADC_Prescaler_Div6
 *             ADC_Prescaler_Div8
 * ����ֵ����
 **/
void adc1_in5_init(uint32_t prescaler);

/**
 * ���ܣ�  ��ȡ ADC1_IN5 ��ת��ֵ 
 * ������  ��
 * ����ֵ��ת��������ֵ
 **/
uint16_t adc1_in5_get_conv_value(void);

/**
 * ���ܣ�  ��ȡ ADC1_IN5 �� n ��ƽ��ת��ֵ 
 * ������  n ת������
 * ����ֵ��ת����ƽ������ֵ
 **/
uint16_t adc1_in5_get_avg_value(int n);

#endif
