#ifndef _DAC_H
#define _DAC_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化 DAC1
 * 参数：  无
 * 返回值：无
 **/
void dac1_init(void);

/**
 * 功能：  向 DAC1 输出数据
 * 参数：  data 要输出的数字信号数据
 * 返回值：无
 **/
void dac1_output_data(uint16_t data);

#endif
	