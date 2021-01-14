#ifndef _RS485_H
#define _RS485_H

#include "bitbanding.h"

#define RS485_RECV 0
#define RS485_SEND 1

#define RS485_CMD(cmd) do { PGout(8) = cmd; } while (0)

/**
 * ���ܣ�  ��ʼ�� rs485������������Ϊ baudrate
 *         Ĭ������ 8 λ����λ������żУ��λ��1 λֹͣλ
 * ������  baudrate ������
 * ����ֵ����
 **/
void rs485_init(uint32_t baudrate);

/**
 * ���ܣ�  �� RS485 ��ȡһ���ַ�
 * ������  ��
 * ����ֵ���ɹ����ػ�ȡ�����ַ���ʧ�ܷ��� EOF
 **/
int rs485_getchar(void);

/**
 * ���ܣ�  �� RS485 ����һ���ַ� c
 * ������  c Ҫ���͵��ַ�
 * ����ֵ���ɹ����ط��͵��ַ���ʧ�ܷ��� EOF
 **/
int rs485_putchar(int c);

#endif

