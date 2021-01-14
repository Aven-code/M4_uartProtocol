
#include "stm32f4xx.h"
#include "rs485.h"
#include "stdio.h"

/*
	查原理图得知：
	
	RS485_TX -> USART2_RX -> PA3，4/6 跳线
	RS485_RX -> USART2_TX -> PA2，3/5 跳线
	RS485_RE -> PG8, 低电平从 RS485 总线接收后发送到 MCU，高电平从 MCU 接收发送到 RS485 总线
	在 MCU 与 RS485 总线之间使用 SP3485 转换芯片
 */
 
static volatile int rs485_recv_ch = EOF;   // 收到的数据
static volatile int rs485_recv_flag = 0;   // 收到数据标志：1 收到，0 未收到

/**
 * 功能：  初始化 rs485，波特率配置为 baudrate
 *         默认配置 8 位数据位，无奇偶校验位，1 位停止位
 * 参数：  baudrate 波特率
 * 返回值：无
 **/
void rs485_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 使能 GPIOA, GPIOG 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOG, ENABLE);
	
	// 使能 USART2 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	// 设置 PA2, PA3复用为 USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	// 配置 PA2, PA3
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;              // 复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;             // 推挽
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;              // 上拉，高电平空闲
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置 PG8
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;   // 输出
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	// 配置 USART2
	// void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
	USART_InitStruct.USART_BaudRate            = baudrate;    // 波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无流控
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // 收发
	USART_InitStruct.USART_Parity              = USART_Parity_No;           // 无奇偶校验
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;          // 1 位停止位
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;       // 8 位数据位
	USART_Init(USART2, &USART_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;    // 中断向量表位置
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // 使能 NVIC 中断
	// 优先级数字越小优先级越高
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;   // 抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;   // 响应(子/次)优先级
	NVIC_Init(&NVIC_InitStruct);
	// void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	USART_ClearITPendingBit(USART2, USART_IT_RXNE | USART_IT_TXE);   // 清空接收非空中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);    // 使能接收非空中断
	
	// 使能 USART2
	// void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	USART_Cmd(USART2, ENABLE);
	
	// 默认为接收状态
	RS485_CMD(RS485_RECV);
}

/**
 * 功能：  从 RS485 获取一个字符
 * 参数：  无
 * 返回值：成功返回获取到的字符，失败返回 EOF
 **/
int rs485_getchar(void)
{
	uint16_t ch = 0;

	// 切换到接收状态
	RS485_CMD(RS485_RECV);
	
	while (rs485_recv_flag == 0);   // 等待数据
	ch = rs485_recv_ch;    // 读取数据
	rs485_recv_ch = EOF;   // 清空数据
	rs485_recv_flag = 0;   // 清空标志

	if ('\r' == ch)   // 收到 '\r' 则转换为 '\n'
	{
		ch = '\n';
	}
	
	// 返回数据
	return ch;
}

/**
 * 功能：  向 RS485 发送一个字符 c
 * 参数：  c 要发送的字符
 * 返回值：成功返回发送的字符，失败返回 EOF
 **/
int rs485_putchar(int c)
{
	// 切换到发送状态
	RS485_CMD(RS485_SEND);
	
	// 等待可发送数据（即等待发送数据缓冲区为空）
	while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TXE));
	
	// 发送数据
	USART_SendData(USART2, c);
	
	// 等待发送完成
	while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC));
	
	// 切换到接收状态
	RS485_CMD(RS485_RECV);
	
	return c;
}

// 中断处理函数的请注意事项：
// 1. 无参
// 2. 无返回值
// 3. 名称必须与中断向量表中完全相同
// 4. 一般不要进行浮点运算
// 5. 一般不要调用输入输出函数
// 6. 一般不要进行延时、休眠
// 4,5,6 点都是为了减少中断处理函数执行时间
// 中断处理函数必须快进快出

// USART2 中断处理(服务)函数
void USART2_IRQHandler(void)
{
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	if (SET == USART_GetITStatus(USART2, USART_IT_RXNE))  // 是接收非空中断
	{
		rs485_recv_ch = USART_ReceiveData(USART2);
		rs485_recv_flag = 1;
		// 清空接收非空中断标志，注意如果不清空中断标志，则无法响应之后的中断
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		//LED0 = !LED0;
	}
}

