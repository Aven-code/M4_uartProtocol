
#include "stm32f4xx.h"
#include "usart.h"
#include "stdio.h"
#include "stdarg.h"
#include "bitbanding.h"

#include "protocol.h"
#include "delay.h"
#include "led.h"
#include "beep.h"
#include <stdlib.h>
#include <string.h>
#include "funcode.h"
#include "main.h"
// NVIC �� SCB ����������� ��en.DM00046982(�ں˱���ֲ�).pdf�� �� 4.3 �� 4.4 �ڣ�
// ��DM00031020_zh(���Ĳο��ֲ�).pdf�� ����û��˵��
// һ��Ӧ�õ��ж����ȼ�����ֻ�ܷ���һ�Σ����ܶ�η��飬���һ���ڳ���ʼʱ�ͽ������ȼ����飬
// ֮���ٸ���

/*
	��ԭ��ͼ�ã�
	
	USB D+/D- --> CH340G D+/D-, RXD/TXD --> USART1 ������ USART1_TX/USART1_RX --> MCU PA9/PA10
	
	��� PA9 �� MCU ���Ϳڣ�PA10 �� MCU ���տڣ����ʹ�� MCU �� USART1 ������д���ͨ�ţ�����Ҫ
	����������Ϊ IO ����
	
 */

// -------------------------------- ʹ�� USART1 ���蹦�� ---------------------------------

#define USART1_USE_INTERRUPT   // ʹ���ж�

#ifdef USART1_USE_INTERRUPT
static volatile int usart1_recv_ch = EOF;   // �յ�������
static volatile int usart1_recv_flag = 0;   // �յ����ݱ�־��1 �յ���0 δ�յ�
#endif

/**
 * ���ܣ�  ��ʼ�� USART1������������Ϊ baudrate
 *         Ĭ������ 8 λ����λ������żУ��λ��1 λֹͣλ
 * ������  baudrate ������
 * ����ֵ����
 **/
void usart1_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	
	// ʹ�� GPIOA ��ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// ʹ�� USART1 ��ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// ���� PA9, PA10 ����Ϊ USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	// ���� PA9, PA10
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;              // ���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;             // ����
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;              // �������ߵ�ƽ����
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// ���� USART1
	// void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
	USART_InitStruct.USART_BaudRate            = baudrate;    // ������
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // ������
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // �շ�
	USART_InitStruct.USART_Parity              = USART_Parity_No;           // ����żУ��
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;          // 1 λֹͣλ
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;       // 8 λ����λ
	USART_Init(USART1, &USART_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;    // �ж�������λ��
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // ʹ�� NVIC �ж�
	// ���ȼ�����ԽС���ȼ�Խ��
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;   // ��ռ���ȼ�
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;   // ��Ӧ(��/��)���ȼ�
	NVIC_Init(&NVIC_InitStruct);
	// void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);   // ��ս��շǿ��ж�
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    // ʹ�ܽ��շǿ��ж�


	// ʹ�� USART1
	// void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	USART_Cmd(USART1, ENABLE);
}

// void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
// uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
// FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
// void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);

/**
 * ���ܣ�  �� USART1 ��ȡһ���ַ�
 * ������  ��
 * ����ֵ���ɹ����ػ�ȡ�����ַ���ʧ�ܷ��� EOF
 **/
int usart1_getchar(void)
{
	uint16_t ch = 0;
	

	while (usart1_recv_flag == 0);   // �ȴ�����
	ch = usart1_recv_ch;    // ��ȡ����
	usart1_recv_ch = EOF;   // �������
	usart1_recv_flag = 0;   // ��ձ�־

	
	
	// ��������
	return ch;
}

/**
 * ���ܣ�  �� USART1 ����һ���ַ� c
 * ������  c Ҫ���͵��ַ�
 * ����ֵ���ɹ����ط��͵��ַ���ʧ�ܷ��� EOF
 **/
int usart1_putchar(int c)
{
	// �ȴ��ɷ������ݣ����ȴ��������ݻ�����Ϊ�գ�
	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	
	// ��������
	USART_SendData(USART1, c);
	
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



// USART1 �жϴ���(����)����
void USART1_IRQHandler(void)
{
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	if (SET == USART_GetITStatus(USART1, USART_IT_RXNE))  // �ǽ��շǿ��ж�
	{ 
		int flag= 0;
		flag = scanfData(); 	//��������жϴ�������
  
		//����Э��,Э�����޾���
		if(flag == -1)
		{
			int i,j;
			//��������1��
			BEEP_ON();
			for(i=0;i<0xffff;i++)
				for(j=0;j<0xff;j++);	
			BEEP_OFF();
		}
		else if(flag == 1)
		{
			int i;
			//Э����ȷ��ȫ����
			LED0_ON();
			LED1_ON();
			LED2_ON();
			LED3_ON();
			for(i=0;i<0xffff;i++);
					 
		}
		
 		USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
	}
}




//---------------------------------------- �ض����׼ IO �� USART1 ------------------------------------
// �� Keil �İ�װĿ¼�µ�һ����Ŀ¼��C:\Keil_v5\ARM\Startup ����һ���ض��� ģ���ļ� Retarget.c
// ���Ը����������ݵ����棺

/******************************************************************************/
/* RETARGET.C: 'Retarget' layer for target-dependent low level functions      */
/******************************************************************************/
/* This file is part of the uVision/ARM development tools.                    */
/* Copyright (c) 2005 Keil Software. All rights reserved.                     */
/* This software may only be used under the terms of a valid, current,        */
/* end user licence from KEIL for a compatible version of KEIL software       */
/* development tools. Nothing else gives you the right to use this software.  */
/******************************************************************************/

#include <stdio.h>
#include <time.h>
#include <rt_misc.h>

// ��ʹ�ð����������������Ż�ʹ�������ض�����
#pragma import(__use_no_semihosting_swi)


//extern int  sendchar(int ch);  /* in Serial.c */
//extern int  getkey(void);      /* in Serial.c */
extern long timeval;           /* in Time.c   */


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

// ���º����ڱ�׼������������н��е��ã��Ӷ�ʵ������������ض���

int fputc(int ch, FILE *f) {
  //return (sendchar(ch));
	return usart1_putchar(ch);   // �� usart1 �ĺ����滻
}

int fgetc(FILE *f) {
  //return (sendchar(getkey()));
	return usart1_getchar();     // �� usart1 �ĺ����滻
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  //sendchar (ch);
	usart1_putchar(ch);          // �� usart1 �ĺ����滻
}


void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}



