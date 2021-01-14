#ifndef _RS485_H
#define _RS485_H

#include "bitbanding.h"

#define RS485_RECV 0
#define RS485_SEND 1

#define RS485_CMD(cmd) do { PGout(8) = cmd; } while (0)

/**
 * 功能：  初始化 rs485，波特率配置为 baudrate
 *         默认配置 8 位数据位，无奇偶校验位，1 位停止位
 * 参数：  baudrate 波特率
 * 返回值：无
 **/
void rs485_init(uint32_t baudrate);

/**
 * 功能：  从 RS485 获取一个字符
 * 参数：  无
 * 返回值：成功返回获取到的字符，失败返回 EOF
 **/
int rs485_getchar(void);

/**
 * 功能：  向 RS485 发送一个字符 c
 * 参数：  c 要发送的字符
 * 返回值：成功返回发送的字符，失败返回 EOF
 **/
int rs485_putchar(int c);

#endif

