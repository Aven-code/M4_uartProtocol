
#include "stm32f4xx.h"
#include "dac.h"

/*
	��ԭ��ͼ�ã�
	
	DCMI_HREF -> PA4/DAC1_OUT
	
	��˿��Խ��� ADC1_IN5 �õ��ĵ�λ�������ģ���ѹ�����ֱ��룬
	ͨ�� DAC1_OUT �����ֱ���ת��Ϊģ���ѹ����� DCMI_HREF ���ţ�ͨ�����ñ������ѹ������֤��
	
	�����Ĳο��ֲᡷ�ġ�12.2 DAC ��Ҫ���ԡ�����������ʾ��
	
	ע�⣺ ʹ�� DAC ͨ�� x ����Ӧ GPIO ���ţ�PA4 �� PA5�����Զ����ӵ�ģ��ת�������
	(DAC_OUTx)��Ϊ�˱�������������ģ�Ӧ���Ƚ� PA4 �� PA5 ��������Ϊģ��ģʽ (AIN)��

 */

/**
 * ���ܣ�  ��ʼ�� DAC1
 * ������  ��
 * ����ֵ����
 **/
void dac1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	DAC_InitTypeDef DAC_InitStruct;
	
	// ʹ�� GPIOA ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// ʹ�� DAC ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_DAC, ENABLE);
	
	// ���� PA4
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AN;   // ģ��ģʽ
	//GPIO_InitStruct.GPIO_OType = ;
	//GPIO_InitStruct.GPIO_Speed = ;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// DAC1 ����
	// ͨ������/���ѡ���������ڲ������κβ�����˲�������
	//DAC_InitStruct.DAC_LFSRUnmask_TriangleAmplitude = ;
	// �����������
	DAC_InitStruct.DAC_OutputBuffer = DAC_OutputBuffer_Disable;
	// �������ת��
	DAC_InitStruct.DAC_Trigger = DAC_Trigger_Software;
	// ����������
	DAC_InitStruct.DAC_WaveGeneration = 0x00;
	DAC_Init(DAC_Channel_1, &DAC_InitStruct);
	
	// ʹ�� DAC1
	DAC_Cmd(DAC_Channel_1, ENABLE);
}

// void DAC_SoftwareTriggerCmd(uint32_t DAC_Channel, FunctionalState NewState);
// void DAC_SetChannel1Data(uint32_t DAC_Align, uint16_t Data);

/**
 * ���ܣ�  �� DAC1 �������
 * ������  data Ҫ����������ź�����
 * ����ֵ����
 **/
void dac1_output_data(uint16_t data)
{
	// ������д�� DHR �Ĵ���
	DAC_SetChannel1Data(DAC_Align_12b_R, data);
	// ͨ�������������ת�������� DHR �е�����ת�Ƶ� DOR �У�����Ӳ���Զ���λ
	DAC_SoftwareTriggerCmd(DAC_Channel_1, ENABLE);
}



