#ifndef _TIM_H
#define _TIM_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ�� TIM2
 * ������  period    �������ڣ����Զ����ؼ���ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         prescaler Ԥ��Ƶֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 * ����ֵ����
 **/ 
void tim2_init(uint16_t period, uint16_t prescaler);

/**
 * ���ܣ�  �ж� TIM2 �Ƿ���������ж�
 * ������  ��
 * ����ֵ�������˸����жϷ��� 1�����򷵻� 0
 **/
int tim2_is_int(void);

/**
 * ���ܣ�  ��ʼ�� TIM14_CH1 �� LED0
 * ������  period    �������ڣ����Զ����ؼ���ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         prescaler Ԥ��Ƶֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 *         compare   �Ƚ�ֵ��ȡֵ��Χ��0x0000 - 0xFFFF
 * ����ֵ����
 **/
void tim14_ch1_led0_init(uint16_t period, uint16_t prescaler, uint16_t compare);

/**
 * ���ܣ�  �޸� TIM14_CH1 �� PWM ����ֵ
 * ������  cnt PWM ����ֵ
 * ����ֵ����
 **/ 
void tim14_ch1_led0_set_pwm_cnt(uint32_t cnt);

/**
 * ���ܣ�  ��ȡ TIM14_CH1 �� PWM ����ֵ
 * ������  ��
 * ����ֵ����
 **/ 
uint32_t tim14_ch1_led0_get_pwm_cnt(void);

#endif
