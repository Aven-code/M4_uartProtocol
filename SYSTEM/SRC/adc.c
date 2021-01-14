
#include "stm32f4xx.h"
#include "adc.h"

/*
	��ԭ��ͼ�ɵã�
	
	STM_ADC -> PA5 / ADC1&2_IN5
	
	���뽫 P10 ���ߵ� 1��2 ������ñ���ӣ���Ϊ ADC �ṩ�ο���ѹ��
	
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
 * ���ܣ�  ��ʼ�� ADC1_IN5
 * ������  prescaler ģ���·ʱ��Ԥ��Ƶֵ����ȡֵ��
 *             ADC_Prescaler_Div2
 *             ADC_Prescaler_Div4
 *             ADC_Prescaler_Div6
 *             ADC_Prescaler_Div8
 * ����ֵ����
 **/
void adc1_in5_init(uint32_t prescaler)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	ADC_InitTypeDef ADC_InitStruct;
	ADC_CommonInitTypeDef ADC_CommonInitStruct;
	
	// ʹ�� ADC1 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1, ENABLE); 
	
	// ʹ�� GPIOA ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// ���� PA5
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;   // ģ�⹦��
	//GPIO_InitStruct.GPIO_OType = ;   // ��������
	//GPIO_InitStruct.GPIO_Speed = ;   // ��������
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// ͨ������
	ADC_CommonInitStruct.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
	ADC_CommonInitStruct.ADC_Mode = ADC_Mode_Independent;   // ���� ADC������˫��Ҳ�������أ�
	ADC_CommonInitStruct.ADC_Prescaler = prescaler;
	ADC_CommonInitStruct.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_20Cycles;
	ADC_CommonInit(&ADC_CommonInitStruct);
	
	// ���� ADC1
	ADC_InitStruct.ADC_ContinuousConvMode = DISABLE;   // ������ת��(����ת��)
	ADC_InitStruct.ADC_DataAlign = ADC_DataAlign_Right;  // �����Ҷ���
	// ���ﲻʹ���ⲿ����ת������˿��Բ�����
	//ADC_InitStruct.ADC_ExternalTrigConv = ADC_ExternalTrigConv_T1_CC1;
	// �������� ADC_CR2 �� EXTEN ��ѡ�� 00����ֹ������⣬��ʾͨ���������ת��
	ADC_InitStruct.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
	ADC_InitStruct.ADC_NbrOfConversion = 1;   // һ������ֻת��һ��ͨ��
	ADC_InitStruct.ADC_Resolution = ADC_Resolution_12b;   // 12 λ�ֱ���
	ADC_InitStruct.ADC_ScanConvMode = DISABLE;  // ��ɨ��ģʽ(��ͨ��ģʽ)
	ADC_Init(ADC1, &ADC_InitStruct);
	
	// ���ת��������־
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	// ʹ�� ADC1
	ADC_Cmd(ADC1, ENABLE);
}

// void ADC_RegularChannelConfig(ADC_TypeDef* ADCx, uint8_t ADC_Channel, uint8_t Rank, uint8_t ADC_SampleTime);
// uint16_t ADC_GetConversionValue(ADC_TypeDef* ADCx);
// void ADC_SoftwareStartConv(ADC_TypeDef* ADCx);
// FlagStatus ADC_GetFlagStatus(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);
// void ADC_ClearFlag(ADC_TypeDef* ADCx, uint8_t ADC_FLAG);

/**
 * ���ܣ�  ��ȡ ADC1_IN5 ��ת��ֵ 
 * ������  ��
 * ����ֵ��ת��������ֵ
 **/
uint16_t adc1_in5_get_conv_value(void)
{
	// ���ù���ͨ��
	ADC_RegularChannelConfig(ADC1, ADC_Channel_5, 1, ADC_SampleTime_480Cycles);
	// �������ת��
	ADC_SoftwareStartConv(ADC1);
	// �ȴ�ת������
	while (RESET == ADC_GetFlagStatus(ADC1, ADC_FLAG_EOC));
	// ���ת��������־
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
	
	// ��ȡת�����
	return ADC_GetConversionValue(ADC1);
}

/**
 * ���ܣ�  ��ȡ ADC1_IN5 �� n ��ƽ��ת��ֵ 
 * ������  n ת������
 * ����ֵ��ת����ƽ������ֵ
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
