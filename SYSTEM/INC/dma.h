#ifndef _DMA_H
#define _DMA_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化 DMA2 数据流 0 通道 0
 * 参数：  src_addr  源存储地址
 *         dest_addr 目标存储地址
 *         size      要传输的字节数
 * 返回值：无
 **/
void dma2_str0_ch0_init(uint32_t src_addr, uint32_t dest_addr, uint32_t size);

/**
 * 功能：  获取 DMA2 数据流 0 通道 0 传输是否完成，并清除完成标志
 * 参数：  无
 * 返回值：完成返回 1，未完成返回 0
 **/
int dma2_str0_ch0_is_end(void);

#endif
	