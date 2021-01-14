
#include "stm32f4xx.h"
#include "rs485.h"
#include "stdio.h"

/*
	��ԭ��ͼ��֪��
	
	RS485_TX -> USART2_RX -> PA3��4/6 ����
	RS485_RX -> USART2_TX -> PA2��3/5 ����
	RS485_RE -> PG8, �͵�ƽ�� RS485 ���߽��պ��͵� MCU���ߵ�ƽ�� MCU ���շ��͵� RS485 ����
	�� MCU �� RS485 ����֮��ʹ�� SP3485 ת��оƬ
 */
 
static volatile int rs485_recv_ch = EOF;   // �յ�������
static volatile int rs485_recv_flag = 0;   // �յ����ݱ�־��1 �յ���0 δ�յ�

/**
 * ���ܣ�  ��ʼ�� rs485������������Ϊ baudrate
 *         Ĭ������ 8 λ����λ������żУ��λ��1 λֹͣλ
 * ������  baudrate ������
 * ����ֵ����
 **/
void rs485_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// ʹ�� GPIOA, GPIOG ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA | RCC_AHB1Periph_GPIOG, ENABLE);
	
	// ʹ�� USART2 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
	
	// ���� PA2, PA3����Ϊ USART2
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
	
	// ���� PA2, PA3
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;              // ���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;             // ����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_2 | GPIO_Pin_3;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;              // �������ߵ�ƽ����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// ���� PG8
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_OUT;   // ���
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	// ���� USART2
	// void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
	USART_InitStruct.USART_BaudRate            = baudrate;    // ������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ������
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // �շ�
	USART_InitStruct.USART_Parity              = USART_Parity_No;           // ����żУ��
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;          // 1 λֹͣλ
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;       // 8 λ����λ
	USART_Init(USART2, &USART_InitStruct);

	NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;    // �ж�������λ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // ʹ�� NVIC �ж�
	// ���ȼ�����ԽС���ȼ�Խ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;   // ��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;   // ��Ӧ(��/��)���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	// void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	USART_ClearITPendingBit(USART2, USART_IT_RXNE | USART_IT_TXE);   // ��ս��շǿ��ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);    // ʹ�ܽ��շǿ��ж�
	
	// ʹ�� USART2
	// void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	USART_Cmd(USART2, ENABLE);
	
	// Ĭ��Ϊ����״̬
	RS485_CMD(RS485_RECV);
}

/**
 * ���ܣ�  �� RS485 ��ȡһ���ַ�
 * ������  ��
 * ����ֵ���ɹ����ػ�ȡ�����ַ���ʧ�ܷ��� EOF
 **/
int rs485_getchar(void)
{
	uint16_t ch = 0;

	// �л�������״̬
	RS485_CMD(RS485_RECV);
	
	while (rs485_recv_flag == 0);   // �ȴ�����
	ch = rs485_recv_ch;    // ��ȡ����
	rs485_recv_ch = EOF;   // �������
	rs485_recv_flag = 0;   // ��ձ�־

	if ('\r' == ch)   // �յ� '\r' ��ת��Ϊ '\n'
	{
		ch = '\n';
	}
	
	// ��������
	return ch;
}

/**
 * ���ܣ�  �� RS485 ����һ���ַ� c
 * ������  c Ҫ���͵��ַ�
 * ����ֵ���ɹ����ط��͵��ַ���ʧ�ܷ��� EOF
 **/
int rs485_putchar(int c)
{
	// �л�������״̬
	RS485_CMD(RS485_SEND);
	
	// �ȴ��ɷ������ݣ����ȴ��������ݻ�����Ϊ�գ�
	while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TXE));
	
	// ��������
	USART_SendData(USART2, c);
	
	// �ȴ��������
	while (RESET == USART_GetFlagStatus(USART2, USART_FLAG_TC));
	
	// �л�������״̬
	RS485_CMD(RS485_RECV);
	
	return c;
}

// �жϴ���������ע�����
// 1. �޲�
// 2. �޷���ֵ
// 3. ���Ʊ������ж�����������ȫ��ͬ
// 4. һ�㲻Ҫ���и�������
// 5. һ�㲻Ҫ���������������
// 6. һ�㲻Ҫ������ʱ������
// 4,5,6 �㶼��Ϊ�˼����жϴ�����ִ��ʱ��
// �жϴ��������������

// USART2 �жϴ���(����)����
void USART2_IRQHandler(void)
{
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	if (SET == USART_GetITStatus(USART2, USART_IT_RXNE))  // �ǽ��շǿ��ж�
	{
		rs485_recv_ch = USART_ReceiveData(USART2);
		rs485_recv_flag = 1;
		// ��ս��շǿ��жϱ�־��ע�����������жϱ�־�����޷���Ӧ֮����ж�
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);
		//LED0 = !LED0;
	}
}

