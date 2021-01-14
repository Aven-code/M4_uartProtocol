
#ifndef _LED_H_
#define _LED_H_

#include "stm32f4xx.h"

//#define LED_USE_ORIGINAL      // 原生方式
//#define LED_USE_STRUCTURE     // 结构体方式
#define LED_USE_LIBRARY       // 库函数方式

/**
 * 功能：  初始化 LED
 * 参数：  无
 * 返回值：无
 **/
void led_init(void);

#ifdef LED_USE_LIBRARY     // 使用库函数方式

// void GPIO_SetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#define LED0_OFF() do { GPIO_SetBits(GPIOF, GPIO_Pin_9); } while (0)
#define LED1_OFF() do { GPIO_SetBits(GPIOF, GPIO_Pin_10); } while (0)
#define LED2_OFF() do { GPIO_SetBits(GPIOE, GPIO_Pin_13); } while (0)
#define LED3_OFF() do { GPIO_SetBits(GPIOE, GPIO_Pin_14); } while (0)

// void GPIO_ResetBits(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#define LED0_ON() do { GPIO_ResetBits(GPIOF, GPIO_Pin_9); } while (0)
#define LED1_ON() do { GPIO_ResetBits(GPIOF, GPIO_Pin_10); } while (0)
#define LED2_ON() do { GPIO_ResetBits(GPIOE, GPIO_Pin_13); } while (0)
#define LED3_ON() do { GPIO_ResetBits(GPIOE, GPIO_Pin_14); } while (0)

#elif defined(LED_USE_STRUCTURE)    // 使用结构体方式

#if 0    // 直接对输出数据寄存器操作

#define LED0_OFF() do { GPIOF->ODR |= 0x1 << 9; } while (0)
#define LED1_OFF() do { GPIOF->ODR |= 0x1 << 10; } while (0)
#define LED2_OFF() do { GPIOE->ODR |= 0x1 << 13; } while (0)
#define LED3_OFF() do { GPIOE->ODR |= 0x1 << 14; } while (0)

#define LED0_ON()  do { GPIOF->ODR &= ~(0x1 << 9); } while (0)
#define LED1_ON()  do { GPIOF->ODR &= ~(0x1 << 10); } while (0)
#define LED2_ON()  do { GPIOE->ODR &= ~(0x1 << 13); } while (0)
#define LED3_ON()  do { GPIOE->ODR &= ~(0x1 << 14); } while (0)

#else    // 通过置位/复位寄存器操作输出数据寄存器

#define LED0_ON()  do { GPIOF->BSRRH |= 0x1 << 9; } while (0)      // 高 16 位清 0
#define LED1_ON()  do { GPIOF->BSRRH |= 0x1 << 10; } while (0)
#define LED2_ON()  do { GPIOE->BSRRH |= 0x1 << 13; } while (0)
#define LED3_ON()  do { GPIOE->BSRRH |= 0x1 << 14; } while (0)

#define LED0_OFF() do { GPIOF->BSRRL |= 0x1 << 9; } while (0)      // 低 16 位置 1
#define LED1_OFF() do { GPIOF->BSRRL |= 0x1 << 10; } while (0)
#define LED2_OFF() do { GPIOE->BSRRL |= 0x1 << 13; } while (0)
#define LED3_OFF() do { GPIOE->BSRRL |= 0x1 << 14; } while (0)

#endif

#elif defined(LED_USE_ORIGINAL)     // 使用原生方式

// 原生方式实现点亮 LED 和熄灭 LED 的宏

#if 1   // 直接对输出数据寄存器操作

// 7.4.6 GPIO 端口输出数据寄存器 (GPIOx_ODR) (x = A..I) 偏移地址 0x14
#define LED0_OFF() do { *((volatile unsigned int *) (0x40021400 + 0x14)) |= 0x1 << 9; } while (0)
#define LED1_OFF() do { *((volatile unsigned int *) (0x40021400 + 0x14)) |= 0x1 << 10; } while (0)
#define LED2_OFF() do { *((volatile unsigned int *) (0x40021000 + 0x14)) |= 0x1 << 13; } while (0)
#define LED3_OFF() do { *((volatile unsigned int *) (0x40021000 + 0x14)) |= 0x1 << 14; } while (0)

#define LED0_ON()  do { *((volatile unsigned int *) (0x40021400 + 0x14)) &= ~(0x1 << 9); } while (0)
#define LED1_ON()  do { *((volatile unsigned int *) (0x40021400 + 0x14)) &= ~(0x1 << 10); } while (0)
#define LED2_ON()  do { *((volatile unsigned int *) (0x40021000 + 0x14)) &= ~(0x1 << 13); } while (0)
#define LED3_ON()  do { *((volatile unsigned int *) (0x40021000 + 0x14)) &= ~(0x1 << 14); } while (0)

#else   // 通过置位/复位寄存器操作输出数据寄存器

// 7.4.7 GPIO 端口置位/复位寄存器 (GPIOx_BSRR) (x = A..I) 偏移地址 0x18
#define LED0_ON()   do { *((volatile unsigned int *) (0x40021400 + 0x18)) |= 0x1 << 25; } while (0)
#define LED1_ON()   do { *((volatile unsigned int *) (0x40021400 + 0x18)) |= 0x1 << 26; } while (0)
#define LED2_ON()   do { *((volatile unsigned int *) (0x40021000 + 0x18)) |= 0x1 << 29; } while (0)
#define LED3_ON()   do { *((volatile unsigned int *) (0x40021000 + 0x18)) |= 0x1 << 30; } while (0)

#define LED0_OFF()  do { *((volatile unsigned int *) (0x40021400 + 0x18)) |= 0x1 << 9; } while (0)
#define LED1_OFF()  do { *((volatile unsigned int *) (0x40021400 + 0x18)) |= 0x1 << 10; } while (0)
#define LED2_OFF()  do { *((volatile unsigned int *) (0x40021000 + 0x18)) |= 0x1 << 13; } while (0)
#define LED3_OFF()  do { *((volatile unsigned int *) (0x40021000 + 0x18)) |= 0x1 << 14; } while (0)

#endif  // 0

#endif  // LED_USE_*

#endif  // _LED_H
