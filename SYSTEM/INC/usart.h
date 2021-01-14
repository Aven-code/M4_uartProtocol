#ifndef _USART_H
#define _USART_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ�� USART1������������Ϊ baudrate
 *         Ĭ������ 8 λ����λ������żУ��λ��1 λֹͣλ
 * ������  baudrate ������
 * ����ֵ����
 **/
void usart1_init(uint32_t baudrate);

/**
 * ���ܣ�  �� USART1 ��ȡһ���ַ�
 * ������  ��
 * ����ֵ���ɹ����ػ�ȡ�����ַ���ʧ�ܷ��� EOF
 **/
int usart1_getchar(void);

/**
 * ���ܣ�  �� USART1 ����һ���ַ� c
 * ������  c Ҫ���͵��ַ�
 * ����ֵ���ɹ����ط��͵��ַ���ʧ�ܷ��� EOF
 **/
int usart1_putchar(int c);

/**
 * ���ܣ�  ģ�±�׼��ʽ��������� printf
 * ������  format ��ʽ���ַ���
 * ����ֵ���ɹ����ش�ӡ���ַ�������ʧ�ܷ��ظ���
 **/
int usart1_printf(const char *format, ...);


#endif








