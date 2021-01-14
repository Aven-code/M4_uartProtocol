#ifndef _USART_H
#define _USART_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化 USART1，波特率配置为 baudrate
 *         默认配置 8 位数据位，无奇偶校验位，1 位停止位
 * 参数：  baudrate 波特率
 * 返回值：无
 **/
void usart1_init(uint32_t baudrate);

/**
 * 功能：  从 USART1 获取一个字符
 * 参数：  无
 * 返回值：成功返回获取到的字符，失败返回 EOF
 **/
int usart1_getchar(void);

/**
 * 功能：  向 USART1 发送一个字符 c
 * 参数：  c 要发送的字符
 * 返回值：成功返回发送的字符，失败返回 EOF
 **/
int usart1_putchar(int c);

/**
 * 功能：  模仿标准格式化输出函数 printf
 * 参数：  format 格式化字符串
 * 返回值：成功返回打印的字符个数，失败返回负数
 **/
int usart1_printf(const char *format, ...);


#endif








