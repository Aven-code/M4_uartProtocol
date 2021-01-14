#ifndef _DELAY_H
#define _DELAY_H

#include "stm32f4xx.h"

#define DELAY_USE_INT    // ʹ���ж���ʱ

void Delay(__IO uint32_t nTime);

#ifndef DELAY_USE_INT

/**
 * ���ܣ�  SysTick ��ʱ����ʼ��
 * ������  systick_clksrc SysTick ʱ��Դѡ��
 *                        SysTick_CLKSource_HCLK     : AHB, 
 *                        SysTick_CLKSource_HCLK_Div8: AHB/8
 * ����ֵ����
 **/
void systick_init(uint32_t systick_clksrc);
	
/**
 * ���ܣ�  ��ʱ nus ΢��
 * ������  nus ΢����
 * ����ֵ����
 **/
void delay_us(uint32_t nus);

/**
 * ���ܣ�  ��ʱ nms ΢��
 * ������  nms ������
 * ����ֵ����
 **/
void delay_ms(uint32_t nms);

#endif

#endif
