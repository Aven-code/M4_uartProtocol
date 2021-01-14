
#include "stm32f4xx.h"
#include "dac.h"

/*
	查原理图得：
	
	DCMI_HREF -> PA4/DAC1_OUT
	
	因此可以将从 ADC1_IN5 得到的电位器输入的模拟电压的数字编码，
	通过 DAC1_OUT 将数字编码转换为模拟电压输出到 DCMI_HREF 引脚，通过万用表测量电压进行验证。
	
	《中文参考手册》的《12.2 DAC 主要特性》节有如下提示：
	
	注意： 使能 DAC 通道 x 后，相应 GPIO 引脚（PA4 或 PA5）将自动连接到模拟转换器输出
	(DAC_OUTx)。为了避免寄生电流消耗，应首先将 PA4 或 PA5 引脚配置为模拟模式 (AIN)。

 */

/**
 * 功能：  初始化 DAC1
 * 参数：  无
 * 返回值：无
 **/
void dac1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	DAC_InitTypeDef DAC_InitStruct;
	
	// 使能 GPIOA 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// 使能 DAC 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	// 配置 PA4
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;   // 模拟模式
	//GPIO_InitStruct.GPIO_OType = ;
	//GPIO_InitStruct.GPIO_Speed = ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// DAC1 配置
	// 通道掩码/振幅选择器，由于不生成任何波，因此不用配置
	//DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = ;
	// 禁用输出缓冲
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	// 软件触发转换
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_Software;
	// 不产生波形
	DAC_InitStruct.DAC_WaveGeneration = 0x00;
	DAC_Init(DAC_Channel_1, &DAC_InitStruct);
	
	// 使能 DAC1
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

// void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
// void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);

/**
 * 功能：  向 DAC1 输出数据
 * 参数：  data 要输出的数字信号数据
 * 返回值：无
 **/
void dac1_output_data(uint16_t data)
{
	// 将数据写入 DHR 寄存器
	DAC_SetChannel1Data(DAC_Align_12b_R, data);
	// 通过软件触发进行转换（即将 DHR 中的数据转移到 DOR 中），由硬件自动复位
	DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
}



