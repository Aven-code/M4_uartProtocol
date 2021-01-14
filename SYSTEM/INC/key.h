#ifndef _KEY_H
#define _KEY_H

#include "stm32f4xx.h"

// 按键编号
#define KEY_0 (1 << 0)
#define KEY_1 (1 << 1)
#define KEY_2 (1 << 2)
#define KEY_3 (1 << 3)

// 按键按下/未按下标志
#define KEY_NO_PRESSED 0
#define KEY_PRESSED    1

#if 0

// 使用位带读取
#define KEY0 PAin(0)
#define KEY1 PEin(2)
#define KEY2 PEin(3)
#define KEY3 PEin(4)

#else

// 使用库函数读取
// uint8_t GPIO_ReadInputDataBit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
#define KEY0 GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)
#define KEY1 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_2)
#define KEY2 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_3)
#define KEY3 GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_4)

#endif  // 0

/**
 * 功能：  初始化按键
 * 参数：  无
 * 返回值：无
 **/
void key_init(void);

/**
 * 功能：  扫描按键 KEY0, KEY1, KEY2, KEY3
 * 参数：  flag 松开标志，1 判断是否松开，0 判断是否按下
 * 返回值：按键 KEY0, KEY1, KEY2, KEY3 的状态：1 - 按下，0 - 未按下
 *         0 位 - KEY0, 1 位 - KEY1, 2 位 - KEY2, 3 位 - KEY3
 **/
uint8_t key_scan(uint8_t flag);

/**
 * 功能：  返回按键状态 key_state 中 key 键是否按下
 * 参数：  key_state 四个按键的状态，应该是调用 key_scan() 的返回结果
 *         key       按键编号：KEY_0, KEY_1, KEY_2, KEY_3
 * 返回值：按下返回 KEY_PRESSED，未按下返回 KEY_NO_PRESSED
 **/
uint8_t key_pressed(uint8_t key_state, uint8_t key);

#endif  // _KEY_H
