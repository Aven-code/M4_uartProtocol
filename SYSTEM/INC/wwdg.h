#ifndef _WWDG_H
#define _WWDG_H

#include "stm32f4xx.h"

// 喂狗
#define WWDG_FEED(Counter) WWDG_SetCounter(Counter)

/**
 * 功能：  初始化独立看门狗
 * 参数：  WWDG_Prescaler 预分频值。可取值：
 *             WWDG_Prescaler_1: WWDG counter clock = (PCLK1/4096)/1
 *             WWDG_Prescaler_2: WWDG counter clock = (PCLK1/4096)/2
 *             WWDG_Prescaler_4: WWDG counter clock = (PCLK1/4096)/4
 *             WWDG_Prescaler_8: WWDG counter clock = (PCLK1/4096)/8
 *         WindowValue    窗口值。必须小于 0x80
 *         Counter        计数值。取值范围：0x40 - 0x7F
 * 返回值：无
 **/
void wwdg_init(uint32_t WWDG_Prescaler, uint8_t WindowValue, uint8_t Counter);

/**
 * 功能：  判断是否发生窗口看门狗中断
 * 参数：  无
 * 返回值：有中断返回 1，否则返回 0
 **/
int wwdg_is_int(void);

#endif
