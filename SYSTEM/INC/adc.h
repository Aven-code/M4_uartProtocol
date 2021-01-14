#ifndef _ADC_H
#define _ADC_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化 ADC1_IN5
 * 参数：  prescaler 模拟电路时钟预分频值。可取值：
 *             ADC_Prescaler_Div2
 *             ADC_Prescaler_Div4
 *             ADC_Prescaler_Div6
 *             ADC_Prescaler_Div8
 * 返回值：无
 **/
void adc1_in5_init(uint32_t prescaler);

/**
 * 功能：  获取 ADC1_IN5 的转换值 
 * 参数：  无
 * 返回值：转换的数字值
 **/
uint16_t adc1_in5_get_conv_value(void);

/**
 * 功能：  获取 ADC1_IN5 的 n 次平均转换值 
 * 参数：  n 转换次数
 * 返回值：转换的平均数字值
 **/
uint16_t adc1_in5_get_avg_value(int n);

#endif
