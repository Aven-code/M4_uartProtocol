#ifndef _IWDG_H
#define _IWDG_H

#include "stm32f4xx.h"

// 喂狗
#define IWDG_FEED IWDG_ReloadCounter

/**
 * 功能：  初始化独立看门狗
 * 参数：  IWDG_Prescaler 预分频值。在 LSI 的基础上进行分频，可取值：
 *             IWDG_Prescaler_4
 *             IWDG_Prescaler_8
 *             IWDG_Prescaler_16
 *             IWDG_Prescaler_32
 *             IWDG_Prescaler_64
 *             IWDG_Prescaler_128
 *             IWDG_Prescaler_256
 *         Reload         重装载值。取值范围为 0x0000 - 0x0FFF
 * 返回值：无
 **/
void iwdg_init(uint8_t IWDG_Prescaler, uint16_t Reload);

#endif
