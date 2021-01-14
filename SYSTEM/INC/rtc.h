#ifndef _RTC_H
#define _RTC_H

#include "stm32f4xx.h"

/**
 * 功能：  初始化实时时钟
 * 参数：  无
 * 返回值：成功返回 SUCCESS，失败返回 ERROR
 **/
ErrorStatus rtc_init(void);

/**
 * 功能：  设置 RTC 时间，默认 24 小时制
 * 参数：  hour 时
 *         min  分
 *         sec  秒
 * 返回值：成功返回 SUCCESS，失败返回 ERROR
 **/
ErrorStatus rtc_set_time(uint32_t hour, uint32_t min, uint32_t sec);

/**
 * 功能：  设置 RTC 日期
 * 参数：  year    年
 *         mon     月
 *         date    日
 *         weekday 星期。星期可取值如下：
 *             RTC_Weekday_Monday
 *             RTC_Weekday_Tuesday
 *             RTC_Weekday_Wednesday
 *             RTC_Weekday_Thursday
 *             RTC_Weekday_Friday
 *             RTC_Weekday_Saturday
 *             RTC_Weekday_Sunday
 * 返回值：成功返回 SUCCESS，失败返回 ERROR
 **/
ErrorStatus rtc_set_date(uint32_t year, uint32_t mon, uint32_t date, uint32_t weekday);

/**
 * 功能：  打印当前日期时间
 * 参数：  无
 * 返回值：无
 **/
void rtc_print_date_time(void);

/**
 * 功能：  设置闹钟 RTC_Alarm
 * 参数：  RTC_Alarm               闹钟选择。可取值：
 *             RTC_Alarm_A 闹钟 A
 *             RTC_Alarm_B 闹钟 B
 *         RTC_AlarmDateWeekDaySel 按日期/星期闹。可取值：
 *             RTC_AlarmDateWeekDaySel_Date     按日期闹
 *             RTC_AlarmDateWeekDaySel_WeekDay  按星期闹
 *         RTC_AlarmDateWeekDay    日期/星期值。
 *             如果是日期则可取 1 - 31；
 *             星期则取 1 - 7，可以使用以下宏定义值：
 *             RTC_Weekday_Monday
 *             RTC_Weekday_Tuesday
 *             RTC_Weekday_Wednesday
 *             RTC_Weekday_Thursday
 *             RTC_Weekday_Friday
 *             RTC_Weekday_Saturday
 *             RTC_Weekday_Sunday
 *         RTC_Hours               小时
 *         RTC_Minutes             分钟
 *         RTC_Seconds             秒钟
 * 返回值：无
 **/
void rtc_set_alarm(uint32_t RTC_Alarm, uint32_t RTC_AlarmDateWeekDaySel, uint8_t RTC_AlarmDateWeekDay,
	uint8_t RTC_Hours, uint8_t RTC_Minutes, uint8_t RTC_Seconds);

/**
 * 功能：  设置闹钟 RTC_Alarm 中断
 * 参数：  RTC_Alarm 闹钟选择。可取值：
 *             RTC_Alarm_A 闹钟 A
 *             RTC_Alarm_B 闹钟 B
 * 返回值：无
 **/
void rtc_set_alarm_int(uint32_t RTC_Alarm);

/**
 * 功能：  返回闹钟 A 是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_alarma(void);

/**
 * 功能：  返回闹钟 B 是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_alarmb(void);

/**
 * 功能：  设置自动唤醒
 * 参数：  RTC_WakeUpCounter 唤醒自动重载值。取值范围为 0x0000 - 0xFFFF
 *         RTC_WakeUpClock   唤醒时钟。可取值：
 *             RTC_WakeUpClock_RTCCLK_Div16     RTCCLK / 16
 *             RTC_WakeUpClock_RTCCLK_Div8      RTCCLK / 8
 *             RTC_WakeUpClock_RTCCLK_Div4      RTCCLK / 4
 *             RTC_WakeUpClock_RTCCLK_Div2      RTCCLK / 2
 *             RTC_WakeUpClock_CK_SPRE_16bits   时间范围为 1s - 约18h
 *             RTC_WakeUpClock_CK_SPRE_17bits   时间范围为 约18h - 约36h
 * 返回值：无
 **/
void rtc_set_wakeup(uint32_t RTC_WakeUpCounter, uint32_t RTC_WakeUpClock);

/**
 * 功能：  设置唤醒中断
 * 参数：  无
 * 返回值：无
 **/
void rtc_set_wakeup_int(void);

/**
 * 功能：  返回唤醒是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_wakeup(void);

#endif
