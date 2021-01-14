#ifndef _DMA_H
#define _DMA_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ�� DMA2 ������ 0 ͨ�� 0
 * ������  src_addr  Դ�洢��ַ
 *         dest_addr Ŀ��洢��ַ
 *         size      Ҫ������ֽ���
 * ����ֵ����
 **/
void dma2_str0_ch0_init(uint32_t src_addr, uint32_t dest_addr, uint32_t size);

/**
 * ���ܣ�  ��ȡ DMA2 ������ 0 ͨ�� 0 �����Ƿ���ɣ��������ɱ�־
 * ������  ��
 * ����ֵ����ɷ��� 1��δ��ɷ��� 0
 **/
int dma2_str0_ch0_is_end(void);

#endif
	