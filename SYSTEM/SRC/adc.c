
#include "stm32f4xx.h"
#include "adc.h"

/*
	查原理图可得：
	
	STM_ADC -> PA5 / ADC1&2_IN5
	
	必须将 P10 跳线的 1，2 用跳线帽连接，以为 ADC 提供参考电压。
	
 */
 
/*
   ) ))
  ( ((  /)
 ,-===-//
|`-===-'|
'       '
 \_____/
 `-----'
*/

/**
 * 功能：  初始化 ADC1_IN5
 * 参数：  prescaler 模拟电路时钟预分频值。可取值：
 *             ADC_Prescaler_Div2
 *             ADC_Prescaler_Div4
 *             ADC_Prescaler_Div6
 *             ADC_Prescaler_Div8
 * 返回值：无
 **/
void adc1_in5_init(uint32_t prescaler)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	
	// 使能 ADC1 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	
	// 使能 GPIOA 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// 配置 PA5
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;   // 模拟功能
	//GPIO_InitStruct.GPIO_OType = ;   // 不需配置
	//GPIO_InitStruct.GPIO_Speed = ;   // 不需配置
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 通用配置
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;   // 独立 ADC（不是双重也不是三重）
	ADC_CommonInitStruct.ADC_Prescaler = prescaler;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	// 配置 ADC1
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;   // 不连续转换(单次转换)
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;  // 数据右对齐
	// 这里不使用外部触发转换，因此可以不配置
	//ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	// 这是配置 ADC_CR2 的 EXTEN 域，选择 00：禁止触发检测，表示通过软件启动转换
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion = 1;   // 一个序列只转换一个通道
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;   // 12 位分辨率
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  // 非扫描模式(单通道模式)
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// 清除转换结束标志
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	// 使能 ADC1
	ADC_Cmd(ADC1, ENABLE);
}

// void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
// uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
// void ADC_SoftwareStartConv(ADC_TypeDef* ADCx);
// FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
// void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);

/**
 * 功能：  获取 ADC1_IN5 的转换值 
 * 参数：  无
 * 返回值：转换的数字值
 **/
uint16_t adc1_in5_get_conv_value(void)
{
	// 配置规则通道
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
	// 软件启动转换
	ADC_SoftwareStartConv(ADC1);
	// 等待转换结束
	while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	// 清除转换结束标志
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	// 获取转换结果
	return ADC_GetConversionValue(ADC1);
}

/**
 * 功能：  获取 ADC1_IN5 的 n 次平均转换值 
 * 参数：  n 转换次数
 * 返回值：转换的平均数字值
 **/
uint16_t adc1_in5_get_avg_value(int n)
{
	uint32_t value = 0;
	int i;
	
	for (i = 0; i < n; i++)
	{
		value += adc1_in5_get_conv_value();
	}
	
	return value / n;
}
