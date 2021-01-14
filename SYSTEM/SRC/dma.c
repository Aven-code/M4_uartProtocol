
#include "stm32f4xx.h"
#include "dma.h"

/**
 * ���ܣ�  ��ʼ�� DMA2 ������ 0 ͨ�� 0
 * ������  src_addr  Դ�洢��ַ
 *         dest_addr Ŀ��洢��ַ
 *         size      Ҫ������ֽ���
 * ����ֵ����
 **/
void dma2_str0_ch0_init(uint32_t src_addr, uint32_t dest_addr, uint32_t size)
{
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// ʹ�� DMA2 ������ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	// ���� DMA2
	DMA_InitStruct.DMA_BufferSize = size;    // ��������С�������ֽ�������ָ��������Ŀ
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToMemory;   // �洢�����洢��
	// ʹ�� FIFO ģʽ���洢�����洢��ģʽ������ʹ��ֱ��ģʽ��ֻ��ʹ�� FIFO ģʽ
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	// ��ֵΪ 1 / 4
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	// �ڴ洢�����洢�������У��洢����ַ�Ĵ�����ֻ�ܴ��Ŀ���ַ
	DMA_InitStruct.DMA_Memory0BaseAddr = dest_addr;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;   // ���δ���
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // ���ݿ��
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;  // ��ַ����
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;   // ʹ����ͨģʽ��ע��洢�����洢��������ʹ��ѭ��ģʽ
	// �ڴ洢�����洢�������У������ַ�Ĵ�����ֻ�ܴ��Դ��ַ
	DMA_InitStruct.DMA_PeripheralBaseAddr = src_addr;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  // ���δ���
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // ���ݿ��
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;  // ��ַ����
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;   // ������ȼ�����
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	
	// �����������жϱ�־
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	// ��������ж�����
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	
	// �ȴ��ϴδ������
	while (ENABLE == DMA_GetCmdStatus(DMA2_Stream0));   // �ȴ� EN λ��Ϊ 0
	
	// ʹ�� DMA2 ������ 0
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	// �ȴ����δ��俪ʼ
	while (DISABLE == DMA_GetCmdStatus(DMA2_Stream0));  // �ȴ� EN λ��Ϊ 1

	// ���� NVIC 
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}

// ������ɱ�־
static volatile int flag = 0;

/**
 * ���ܣ�  DMA2 ������ 0 �жϴ������
 * ������  ��
 * ����ֵ����
 **/
void DMA2_Stream0_IRQHandler(void)
{
	// �� DMA2 ������ 0 ͨ�� 0 �Ĵ�������ж�
	if (SET == DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		flag = 1;
		// �����������жϱ�־
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

/**
 * ���ܣ�  ��ȡ DMA2 ������ 0 ͨ�� 0 �����Ƿ���ɣ��������ɱ�־
 * ������  ��
 * ����ֵ����ɷ��� 1��δ��ɷ��� 0
 **/
int dma2_str0_ch0_is_end(void)
{
	if (flag)
	{
		flag = 0;
		return 1;
	}
	
	return flag;
}

