
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
// NVIC 和 SCB 的相关内容在 《en.DM00046982(内核编程手册).pdf》 中 4.3 和 4.4 节，
// 《DM00031020_zh(中文参考手册).pdf》 中则没有说明
// 一个应用的中断优先级分组只能分组一次，不能多次分组，因此一般在程序开始时就进行优先级分组，
// 之后不再更改

/*
	查原理图得：
	
	USB D+/D- --> CH340G D+/D-, RXD/TXD --> USART1 跳线组 USART1_TX/USART1_RX --> MCU PA9/PA10
	
	因此 PA9 是 MCU 发送口，PA10 是 MCU 接收口，如果使用 MCU 的 USART1 外设进行串口通信，则需要
	将它们配置为 IO 复用
	
 */

// -------------------------------- 使用 USART1 外设功能 ---------------------------------

#define USART1_USE_INTERRUPT   // 使用中断

#ifdef USART1_USE_INTERRUPT
static volatile int usart1_recv_ch = EOF;   // 收到的数据
static volatile int usart1_recv_flag = 0;   // 收到数据标志：1 收到，0 未收到
#endif

/**
 * 功能：  初始化 USART1，波特率配置为 baudrate
 *         默认配置 8 位数据位，无奇偶校验位，1 位停止位
 * 参数：  baudrate 波特率
 * 返回值：无
 **/
void usart1_init(uint32_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	USART_InitTypeDef USART_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;

	
	// 使能 GPIOA 的时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
	
	// 使能 USART1 的时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	
	// 设置 PA9, PA10 复用为 USART1
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
	
	// 配置 PA9, PA10
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;              // 复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;             // 推挽
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;              // 上拉，高电平空闲
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOA, &GPIO_InitStruct);
	
	// 配置 USART1
	// void USART_Init(USART_TypeDef* USARTx, USART_InitTypeDef* USART_InitStruct);
	USART_InitStruct.USART_BaudRate            = baudrate;    // 波特率
	USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;  // 无流控
	USART_InitStruct.USART_Mode                = USART_Mode_Rx | USART_Mode_Tx;   // 收发
	USART_InitStruct.USART_Parity              = USART_Parity_No;           // 无奇偶校验
	USART_InitStruct.USART_StopBits            = USART_StopBits_1;          // 1 位停止位
	USART_InitStruct.USART_WordLength          = USART_WordLength_8b;       // 8 位数据位
	USART_Init(USART1, &USART_InitStruct);
	
	NVIC_InitStruct.NVIC_IRQChannel = USART1_IRQn;    // 中断向量表位置
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;      // 使能 NVIC 中断
	// 优先级数字越小优先级越高
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 2;   // 抢占优先级
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;   // 响应(子/次)优先级
	NVIC_Init(&NVIC_InitStruct);
	// void USART_ITConfig(USART_TypeDef* USARTx, uint16_t USART_IT, FunctionalState NewState);
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	USART_ClearITPendingBit(USART1, USART_IT_RXNE);   // 清空接收非空中断
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);    // 使能接收非空中断


	// 使能 USART1
	// void USART_Cmd(USART_TypeDef* USARTx, FunctionalState NewState);
	USART_Cmd(USART1, ENABLE);
}

// void USART_SendData(USART_TypeDef* USARTx, uint16_t Data);
// uint16_t USART_ReceiveData(USART_TypeDef* USARTx);
// FlagStatus USART_GetFlagStatus(USART_TypeDef* USARTx, uint16_t USART_FLAG);
// void USART_ClearFlag(USART_TypeDef* USARTx, uint16_t USART_FLAG);

/**
 * 功能：  从 USART1 获取一个字符
 * 参数：  无
 * 返回值：成功返回获取到的字符，失败返回 EOF
 **/
int usart1_getchar(void)
{
	uint16_t ch = 0;
	

	while (usart1_recv_flag == 0);   // 等待数据
	ch = usart1_recv_ch;    // 读取数据
	usart1_recv_ch = EOF;   // 清空数据
	usart1_recv_flag = 0;   // 清空标志

	
	
	// 返回数据
	return ch;
}

/**
 * 功能：  向 USART1 发送一个字符 c
 * 参数：  c 要发送的字符
 * 返回值：成功返回发送的字符，失败返回 EOF
 **/
int usart1_putchar(int c)
{
	// 等待可发送数据（即等待发送数据缓冲区为空）
	while (RESET == USART_GetFlagStatus(USART1, USART_FLAG_TXE));
	
	// 发送数据
	USART_SendData(USART1, c);
	
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



// USART1 中断处理(服务)函数
void USART1_IRQHandler(void)
{
	// ITStatus USART_GetITStatus(USART_TypeDef* USARTx, uint16_t USART_IT);
	// void USART_ClearITPendingBit(USART_TypeDef* USARTx, uint16_t USART_IT);
	if (SET == USART_GetITStatus(USART1, USART_IT_RXNE))  // 是接收非空中断
	{ 
		int flag= 0;
		flag = scanfData(); 	//这个放在中断处理函数中
  
		//测试协议,协议有无警告
		if(flag == -1)
		{
			int i,j;
			//蜂鸣器响1声
			BEEP_ON();
			for(i=0;i<0xffff;i++)
				for(j=0;j<0xff;j++);	
			BEEP_OFF();
		}
		else if(flag == 1)
		{
			int i;
			//协议正确完全无误
			LED0_ON();
			LED1_ON();
			LED2_ON();
			LED3_ON();
			for(i=0;i<0xffff;i++);
					 
		}
		
 		USART_ClearITPendingBit(USART1, USART_IT_RXNE); 
	}
}




//---------------------------------------- 重定向标准 IO 到 USART1 ------------------------------------
// 在 Keil 的安装目录下的一个子目录：C:\Keil_v5\ARM\Startup 下有一个重定向 模板文件 Retarget.c
// 可以复制它的内容到下面：

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

// 不使用半主机函数，这样才会使用以下重定向函数
#pragma import(__use_no_semihosting_swi)


//extern int  sendchar(int ch);  /* in Serial.c */
//extern int  getkey(void);      /* in Serial.c */
extern long timeval;           /* in Time.c   */


struct __FILE { int handle; /* Add whatever you need here */ };
FILE __stdout;
FILE __stdin;

// 以下函数在标准输入输出函数中进行调用，从而实现输入输出的重定向

int fputc(int ch, FILE *f) {
  //return (sendchar(ch));
	return usart1_putchar(ch);   // 用 usart1 的函数替换
}

int fgetc(FILE *f) {
  //return (sendchar(getkey()));
	return usart1_getchar();     // 用 usart1 的函数替换
}


int ferror(FILE *f) {
  /* Your implementation of ferror */
  return EOF;
}


void _ttywrch(int ch) {
  //sendchar (ch);
	usart1_putchar(ch);          // 用 usart1 的函数替换
}


void _sys_exit(int return_code) {
  while (1);    /* endless loop */
}



