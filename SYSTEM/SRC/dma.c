
#include "stm32f4xx.h"
#include "dma.h"

/**
 * 功能：  初始化 DMA2 数据流 0 通道 0
 * 参数：  src_addr  源存储地址
 *         dest_addr 目标存储地址
 *         size      要传输的字节数
 * 返回值：无
 **/
void dma2_str0_ch0_init(uint32_t src_addr, uint32_t dest_addr, uint32_t size)
{
	DMA_InitTypeDef DMA_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 使能 DMA2 控制器时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA2, ENABLE);
	
	// 配置 DMA2
	DMA_InitStruct.DMA_BufferSize = size;    // 缓冲区大小，不是字节数，是指数据项数目
	DMA_InitStruct.DMA_Channel = DMA_Channel_0;
	DMA_InitStruct.DMA_DIR = DMA_DIR_MemoryToMemory;   // 存储器到存储器
	// 使能 FIFO 模式，存储器到存储器模式不允许使用直接模式，只能使用 FIFO 模式
	DMA_InitStruct.DMA_FIFOMode = DMA_FIFOMode_Enable;
	// 阈值为 1 / 4
	DMA_InitStruct.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull;
	// 在存储器到存储器事务中，存储器地址寄存器中只能存放目标地址
	DMA_InitStruct.DMA_Memory0BaseAddr = dest_addr;
	DMA_InitStruct.DMA_MemoryBurst = DMA_MemoryBurst_Single;   // 单次传输
	DMA_InitStruct.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;  // 数据宽度
	DMA_InitStruct.DMA_MemoryInc = DMA_MemoryInc_Enable;  // 地址自增
	DMA_InitStruct.DMA_Mode = DMA_Mode_Normal;   // 使用普通模式，注意存储器到存储器事务不能使用循环模式
	// 在存储器到存储器事务中，外设地址寄存器中只能存放源地址
	DMA_InitStruct.DMA_PeripheralBaseAddr = src_addr;
	DMA_InitStruct.DMA_PeripheralBurst = DMA_PeripheralBurst_Single;  // 单次传输
	DMA_InitStruct.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;  // 数据宽度
	DMA_InitStruct.DMA_PeripheralInc = DMA_PeripheralInc_Enable;  // 地址自增
	DMA_InitStruct.DMA_Priority = DMA_Priority_High;   // 软件优先级：高
	DMA_Init(DMA2_Stream0, &DMA_InitStruct);
	
	// 清除传输完成中断标志
	DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	// 传输完成中断配置
	DMA_ITConfig(DMA2_Stream0, DMA_IT_TC, ENABLE);
	
	// 等待上次传输结束
	while (ENABLE == DMA_GetCmdStatus(DMA2_Stream0));   // 等待 EN 位变为 0
	
	// 使能 DMA2 数据流 0
	DMA_Cmd(DMA2_Stream0, ENABLE);
	
	// 等待本次传输开始
	while (DISABLE == DMA_GetCmdStatus(DMA2_Stream0));  // 等待 EN 位变为 1

	// 配置 NVIC 
	NVIC_InitStruct.NVIC_IRQChannel = DMA2_Stream0_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_Init(&NVIC_InitStruct);
}

// 传输完成标志
static volatile int flag = 0;

/**
 * 功能：  DMA2 数据流 0 中断处理程序
 * 参数：  无
 * 返回值：无
 **/
void DMA2_Stream0_IRQHandler(void)
{
	// 是 DMA2 数据流 0 通道 0 的传输完成中断
	if (SET == DMA_GetITStatus(DMA2_Stream0, DMA_IT_TCIF0))
	{
		flag = 1;
		// 清除传输完成中断标志
		DMA_ClearITPendingBit(DMA2_Stream0, DMA_IT_TCIF0);
	}
}

/**
 * 功能：  获取 DMA2 数据流 0 通道 0 传输是否完成，并清除完成标志
 * 参数：  无
 * 返回值：完成返回 1，未完成返回 0
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

