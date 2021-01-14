#ifndef _BITBANDING_H
#define _BITBANDING_H

#include "stm32f4xx.h"

// λ�������һ��λ��ӳ�䵽λ�����������е�һ����
// ��ʹֻҪ��λ�����������һ����д 1 �� 0 �Ϳ��ԶԶ�Ӧ��λ�������һ��λд 1 �� 0
// ͬ��������Ҳ���Եõ��Ǹ�λ��ֵ��0 �� 1��

// λ������ÿ��λӳ�䵽λ������������ֵ�ַ��ʽ��
// bit_word_offset = (byte_offset x 32) + (bit_number x 4)
// bit_word_addr = bit_band_base + bit_word_offset
//

#define BIT_WORD_OFFSET(byte_offset, bit_number) ((byte_offset) << 5) + ((bit_number) << 2)
#define BIT_WORD_ADDR(bit_band_base, byte_offset, bit_number) \
	((bit_band_base) + 0x02000000 + BIT_WORD_OFFSET(byte_offset, bit_number))

//#define PF9 *((volatile unsigned int *) BIT_WORD_ADDR(0x40000000, 0x00021400 + 0x14, 9))
// ��� PF9 ��λ����ַӳ��Ϊλ��������ַ��
// 0x42000000 + 0x00021414 * 32 + 0x9 * 4
// 0x42000000 + (0x00021414 << 5) + (0x9 << 2)
// 0x42000000 + 0x00428280 + 0x24 = 0x424282A4

// ������� IO ���������ݼĴ���
#define PAin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOA_BASE & 0xF0000000, (GPIOA_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PBin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOB_BASE & 0xF0000000, (GPIOB_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PCin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOC_BASE & 0xF0000000, (GPIOC_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PDin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOD_BASE & 0xF0000000, (GPIOD_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PEin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOE_BASE & 0xF0000000, (GPIOE_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PFin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOF_BASE & 0xF0000000, (GPIOF_BASE & 0x000FFFFF) + 0x10, bit_number))
#define PGin(bit_number)  *((volatile unsigned int *) BIT_WORD_ADDR(GPIOG_BASE & 0xF0000000, (GPIOG_BASE & 0x000FFFFF) + 0x10, bit_number))

// ������� IO ��������ݼĴ���
#define PAout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOA_BASE & 0xF0000000, (GPIOA_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PBout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOB_BASE & 0xF0000000, (GPIOB_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PCout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOC_BASE & 0xF0000000, (GPIOC_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PDout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOD_BASE & 0xF0000000, (GPIOD_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PEout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOE_BASE & 0xF0000000, (GPIOE_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PFout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOF_BASE & 0xF0000000, (GPIOF_BASE & 0x000FFFFF) + 0x14, bit_number))
#define PGout(bit_number) *((volatile unsigned int *) BIT_WORD_ADDR(GPIOG_BASE & 0xF0000000, (GPIOG_BASE & 0x000FFFFF) + 0x14, bit_number))

#define LED0 PFout(9)
#define LED1 PFout(10)
#define LED2 PEout(13)
#define LED3 PEout(14)

#endif
