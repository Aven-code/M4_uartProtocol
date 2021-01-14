#ifndef _TIM_H
#define _TIM_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化 TIM2
 * 参数：  period    计数周期，即自动重载计数值。取值范围：0x0000 - 0xFFFF
 *         prescaler 预分频值。取值范围：0x0000 - 0xFFFF
 * 返回值：无
 **/ 
void tim2_init(uint16_t period, uint16_t prescaler);

/**
 * 功能：  判断 TIM2 是否产生更新中断
 * 参数：  无
 * 返回值：产生了更新中断返回 1，否则返回 0
 **/
int tim2_is_int(void);

/**
 * 功能：  初始化 TIM14_CH1 和 LED0
 * 参数：  period    计数周期，即自动重载计数值。取值范围：0x0000 - 0xFFFF
 *         prescaler 预分频值。取值范围：0x0000 - 0xFFFF
 *         compare   比较值。取值范围：0x0000 - 0xFFFF
 * 返回值：无
 **/
void tim14_ch1_led0_init(uint16_t period, uint16_t prescaler, uint16_t compare);

/**
 * 功能：  修改 TIM14_CH1 的 PWM 计数值
 * 参数：  cnt PWM 计数值
 * 返回值：无
 **/ 
void tim14_ch1_led0_set_pwm_cnt(uint32_t cnt);

/**
 * 功能：  获取 TIM14_CH1 的 PWM 计数值
 * 参数：  无
 * 返回值：无
 **/ 
uint32_t tim14_ch1_led0_get_pwm_cnt(void);

#endif
