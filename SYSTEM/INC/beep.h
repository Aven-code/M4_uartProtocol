#ifndef _BEEP_H
#define _BEEP_H

#include "bitbanding.h"

/**
 * 功能：  初始化蜂鸣器
 * 参数：  无
 * 返回值：无
 **/
void beep_init(void);

/**
 * 功能：  蜂鸣器鸣叫 count 次 
 * 参数：  t1 鸣叫时长(ms)
 *         t2 不鸣叫时长(ms)
 * 返回值：无
 **/
void beeps(int count, int t1, int t2);

// 定义蜂鸣器鸣叫和不鸣叫宏
#define BEEP_OFF() do { PFout(8) = 0; } while(0)
#define BEEP_ON()  do { PFout(8) = 1; } while(0)

#endif
