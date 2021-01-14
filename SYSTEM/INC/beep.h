#ifndef _BEEP_H
#define _BEEP_H

#include "bitbanding.h"

/**
 * ���ܣ�  ��ʼ��������
 * ������  ��
 * ����ֵ����
 **/
void beep_init(void);

/**
 * ���ܣ�  ���������� count �� 
 * ������  t1 ����ʱ��(ms)
 *         t2 ������ʱ��(ms)
 * ����ֵ����
 **/
void beeps(int count, int t1, int t2);

// ������������кͲ����к�
#define BEEP_OFF() do { PFout(8) = 0; } while(0)
#define BEEP_ON()  do { PFout(8) = 1; } while(0)

#endif
