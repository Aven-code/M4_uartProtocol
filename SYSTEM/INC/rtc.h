#ifndef _RTC_H
#define _RTC_H

#include "stm32f4xx.h"

/**
 * ���ܣ�  ��ʼ��ʵʱʱ��
 * ������  ��
 * ����ֵ���ɹ����� SUCCESS��ʧ�ܷ��� ERROR
 **/
ErrorStatus rtc_init(void);

/**
 * ���ܣ�  ���� RTC ʱ�䣬Ĭ�� 24 Сʱ��
 * ������  hour ʱ
 *         min  ��
 *         sec  ��
 * ����ֵ���ɹ����� SUCCESS��ʧ�ܷ��� ERROR
 **/
ErrorStatus rtc_set_time(uint32_t hour, uint32_t min, uint32_t sec);

/**
 * ���ܣ�  ���� RTC ����
 * ������  year    ��
 *         mon     ��
 *         date    ��
 *         weekday ���ڡ����ڿ�ȡֵ���£�
 *             RTC_Weekday_Monday
 *             RTC_Weekday_Tuesday
 *             RTC_Weekday_Wednesday
 *             RTC_Weekday_Thursday
 *             RTC_Weekday_Friday
 *             RTC_Weekday_Saturday
 *             RTC_Weekday_Sunday
 * ����ֵ���ɹ����� SUCCESS��ʧ�ܷ��� ERROR
 **/
ErrorStatus rtc_set_date(uint32_t year, uint32_t mon, uint32_t date, uint32_t weekday);

/**
 * ���ܣ�  ��ӡ��ǰ����ʱ��
 * ������  ��
 * ����ֵ����
 **/
void rtc_print_date_time(void);

/**
 * ���ܣ�  �������� RTC_Alarm
 * ������  RTC_Alarm               ����ѡ�񡣿�ȡֵ��
 *             RTC_Alarm_A ���� A
 *             RTC_Alarm_B ���� B
 *         RTC_AlarmDateWeekDaySel ������/�����֡���ȡֵ��
 *             RTC_AlarmDateWeekDaySel_Date     ��������
 *             RTC_AlarmDateWeekDaySel_WeekDay  ��������
 *         RTC_AlarmDateWeekDay    ����/����ֵ��
 *             ������������ȡ 1 - 31��
 *             ������ȡ 1 - 7������ʹ�����º궨��ֵ��
 *             RTC_Weekday_Monday
 *             RTC_Weekday_Tuesday
 *             RTC_Weekday_Wednesday
 *             RTC_Weekday_Thursday
 *             RTC_Weekday_Friday
 *             RTC_Weekday_Saturday
 *             RTC_Weekday_Sunday
 *         RTC_Hours               Сʱ
 *         RTC_Minutes             ����
 *         RTC_Seconds             ����
 * ����ֵ����
 **/
void rtc_set_alarm(uint32_t RTC_Alarm, uint32_t RTC_AlarmDateWeekDaySel, uint8_t RTC_AlarmDateWeekDay,
	uint8_t RTC_Hours, uint8_t RTC_Minutes, uint8_t RTC_Seconds);

/**
 * ���ܣ�  �������� RTC_Alarm �ж�
 * ������  RTC_Alarm ����ѡ�񡣿�ȡֵ��
 *             RTC_Alarm_A ���� A
 *             RTC_Alarm_B ���� B
 * ����ֵ����
 **/
void rtc_set_alarm_int(uint32_t RTC_Alarm);

/**
 * ���ܣ�  �������� A �Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_alarma(void);

/**
 * ���ܣ�  �������� B �Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_alarmb(void);

/**
 * ���ܣ�  �����Զ�����
 * ������  RTC_WakeUpCounter �����Զ�����ֵ��ȡֵ��ΧΪ 0x0000 - 0xFFFF
 *         RTC_WakeUpClock   ����ʱ�ӡ���ȡֵ��
 *             RTC_WakeUpClock_RTCCLK_Div16     RTCCLK / 16
 *             RTC_WakeUpClock_RTCCLK_Div8      RTCCLK / 8
 *             RTC_WakeUpClock_RTCCLK_Div4      RTCCLK / 4
 *             RTC_WakeUpClock_RTCCLK_Div2      RTCCLK / 2
 *             RTC_WakeUpClock_CK_SPRE_16bits   ʱ�䷶ΧΪ 1s - Լ18h
 *             RTC_WakeUpClock_CK_SPRE_17bits   ʱ�䷶ΧΪ Լ18h - Լ36h
 * ����ֵ����
 **/
void rtc_set_wakeup(uint32_t RTC_WakeUpCounter, uint32_t RTC_WakeUpClock);

/**
 * ���ܣ�  ���û����ж�
 * ������  ��
 * ����ֵ����
 **/
void rtc_set_wakeup_int(void);

/**
 * ���ܣ�  ���ػ����Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_wakeup(void);

#endif
