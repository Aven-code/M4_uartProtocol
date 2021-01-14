
#include "rtc.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "led.h"

// RTC �жϱ�־���ע�⣺
// 1. ���������ӵ� EXTI 17 �ߣ���˼�Ҫ��� RTC �����жϱ�־��
//    ��Ҫ��� EXTI 17 ���жϱ�־
//    ͬ�����������ӵ� EXTI 22 �ߣ���˼�Ҫ��� RTC �����жϱ�־��
//    ��Ҫ��� EXTI 22 ���жϱ�־
// ��� EXTI 17 �ж����жϱ�־: EXTI_ClearITPendingBit(EXTI_Line17);
// ������� A ���жϱ�־:       RTC_ClearITPendingBit(RTC_IT_ALRA);
// ��� EXTI 22 �ж����жϱ�־: EXTI_ClearITPendingBit(EXTI_Line22);
// ������ѵ��жϱ�־:          RTC_ClearITPendingBit(RTC_IT_WUT);
// ���ϱ�־�ڳ�ʼ��ʱ���Լ����жϴ����ж�Ҫ���

// 2. ���Ӻͻ��ѱ�־�������
//    �ڲ�ʹ���жϵ�����£����Ӻͻ���Ҳ���ڱ�־λ������Ҳ��Ҫ���
// ������� A ��־: RTC_ClearFlag(RTC_FLAG_ALRAF);
// ������ѱ�־:    RTC_ClearFlag(RTC_FLAG_WUTF);

// �ȴ� LSE ������ʱֵ
#define LSE_READY_TIMEOUT  0xFFFFFFUL
// ���� RTC �ѳ�ʼ�����ı�־�Ĵ���
#define RTC_INIT_REG RTC_BKP_DR0
// RTC �ѳ�ʼ����־
#define RTC_INITIALIZED_FLAG 0x87654321

/**
 * ���ܣ�  ��ʼ��ʵʱʱ��
 * ������  ��
 * ����ֵ���ɹ����� SUCCESS��ʧ�ܷ��� ERROR
 **/
ErrorStatus rtc_init(void)
{
	uint32_t count = 0;
	RTC_InitTypeDef RTC_InitStruct;
	
	// ʹ�� PWR ����ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	// void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
	// void        RCC_RTCCLKCmd(FunctionalState NewState);
	// ʹ�ܱ��������
	// void PWR_BackupAccessCmd(FunctionalState NewState);
	PWR_BackupAccessCmd(ENABLE);
	
	// ʹ�� RTC ����ʱ��
	RCC_RTCCLKCmd(ENABLE);
	
	// �ж�֮ǰ�Ƿ��ʼ�����ˣ�����ѳ�ʼ�����ˣ���ֱ�ӷ���
	if (RTC_INITIALIZED_FLAG == RTC_ReadBackupRegister(RTC_INIT_REG))
	{
		return SUCCESS;
	}
	
	// ���� LSE ʱ��Դ
	// void        RCC_LSEConfig(uint8_t RCC_LSE);
	RCC_LSEConfig(RCC_LSE_ON);
	
	// �ȴ�ʱ��Դ������RCC->BDCR �� [1] λ��Ϊ 1
	while (!(RCC->BDCR & 0x2) && (count < LSE_READY_TIMEOUT))
	{
		count++;
	}
	if (count == LSE_READY_TIMEOUT)  // ��ʱ
	{
			return ERROR;
	}
	
	// ѡ�� LSE ��Ϊ RTC ʱ��Դ
	// void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	// ��ʼ�� RTC
	RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
	RTC_InitStruct.RTC_SynchPrediv = 0xFF;
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	// ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
	RTC_Init(&RTC_InitStruct);
	
	// д���ѳ�ʼ����־��������Ĵ���
	// void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data);
	// ����
	RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
	RTC_WriteBackupRegister(RTC_INIT_REG, RTC_INITIALIZED_FLAG);
	// ����
	RTC->WPR = 0xFF;
	
	return SUCCESS;
}

/**
 * ���ܣ�  ���� RTC ʱ�䣬Ĭ�� 24 Сʱ��
 * ������  hour ʱ
 *         min  ��
 *         sec  ��
 * ����ֵ���ɹ����� SUCCESS��ʧ�ܷ��� ERROR
 **/
ErrorStatus rtc_set_time(uint32_t hour, uint32_t min, uint32_t sec)
{
	RTC_TimeTypeDef RTC_TimeStruct;
	
	RTC_TimeStruct.RTC_H12 = RTC_H12_AM;
	RTC_TimeStruct.RTC_Hours = hour;
	RTC_TimeStruct.RTC_Minutes = min;
	RTC_TimeStruct.RTC_Seconds = sec;
	// ErrorStatus RTC_SetTime(uint32_t RTC_Format, RTC_TimeTypeDef* RTC_TimeStruct);
	return RTC_SetTime(RTC_Format_BIN, &RTC_TimeStruct);
}

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
ErrorStatus rtc_set_date(uint32_t year, uint32_t mon, uint32_t date, uint32_t weekday)
{
	RTC_DateTypeDef RTC_DateStruct;

	RTC_DateStruct.RTC_Year = year % 100;
	RTC_DateStruct.RTC_Month = mon;
	RTC_DateStruct.RTC_Date = date;
	RTC_DateStruct.RTC_WeekDay = weekday;
	// ErrorStatus RTC_SetDate(uint32_t RTC_Format, RTC_DateTypeDef* RTC_DateStruct);
	return RTC_SetDate(RTC_Format_BIN, &RTC_DateStruct);
}

/**
 * ���ܣ�  ��ӡ��ǰ����ʱ��
 * ������  ��
 * ����ֵ����
 **/
void rtc_print_date_time(void)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	
	/*
		�����ֲ� ��23.3.6 ��ȡ��������
		�� RTC_CR �Ĵ����е� BYPSHAD ����λ����ʱ������Ӱ�ӼĴ�����ȡ����ʱ��
		ÿ�ν������Ĵ����е�ֵ���Ƶ� RTC_SSR�� RTC_TR �� RTC_DR Ӱ�ӼĴ���ʱ�� 
		RTC_ISR �Ĵ����е� RSF λ������ 1��ÿ���� TRCCLK ����ִ��һ�θ��ơ�Ϊ
		ȷ���� 3 ��ֵ����ͬһʱ�̵㣬��ȡ RTC_SSR �� RTC_TR ʱ�������߽�����
		Ӱ�ӼĴ����е�ֵ��ֱ����ȡ RTC_DR��
		Ϊ�������������ִ�ж����ʵ�ʱ����С�� 2 �� RTCCLK ���ڣ���һ�ζ�ȡ
		����֮�����ͨ������� RSF ���㣬�����������ȴ��� RSF �� 1 ֮��ſ�
		�ٴζ�ȡ RTC_SSR�� RTC_TR �� RTC_DR	�Ĵ�����
		ErrorStatus RTC_WaitForSynchro(void) ������������ RSF λ���ȴ���� 1 ��
	 */
	
	// ��ȡ����ʱ��ʱ�������ȶ�ȡʱ���ٶ����ڣ�����������
	RTC_GetTime(RTC_Format_BIN, &time);
	RTC_GetDate(RTC_Format_BIN, &date);
	
	RTC_WaitForSynchro();
	
	printf("%04d-%02d-%02d %02d:%02d:%02d %d", 
		date.RTC_Year + 2000, date.RTC_Month, date.RTC_Date, 
		time.RTC_Hours, time.RTC_Minutes, time.RTC_Seconds,
		date.RTC_WeekDay);
}



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
	uint8_t RTC_Hours, uint8_t RTC_Minutes, uint8_t RTC_Seconds)
{
	/*
	�������
  Ҫ�Կɱ�̵����ӣ����� A ������ B�����б�̻���£�����ִ�����ƵĲ��裺
  1. �� RTC_CR �Ĵ����е� ALRAE �� ALRBE λ�����Խ�ֹ���� A ������ B��
  2. ��ѯ RTC_ISR �Ĵ����е� ALRAWF �� ALRBWF λ��ֱ������һ���� 1����ȷ��
	   ���ӼĴ������Է��ʡ���Լ��Ҫ 2 �� RTCCLK ʱ�����ڣ�����ʱ��ͬ������
  3. ������� A ������ B �Ĵ�����RTC_ALRMASSR/RTC_ALRMAR �� RTC_ALRMBSSR/
	   RTC_ALRMBR����
  4. �� RTC_CR �Ĵ����е� ALRAE �� ALRBE λ�� 1 ���ٴ�ʹ������ A ������ B��
	 */
	RTC_AlarmTypeDef RTC_AlarmStruct;
	
	// �������ӣ����ȴ� RTC �Ĵ����ɷ���
	RTC_AlarmCmd(RTC_Alarm, DISABLE);
	
	// ��������
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel;
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = RTC_AlarmDateWeekDay;
	// �������룬ָ������/�����֣�����Сʱ�����ӡ�����֣�Ҳ����ȫ��ƥ����
	// RTC_AlarmMask_None ȫ��ƥ�䣬RTC_AlarmMask_All ȫ���޹�
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;   // ȫ��ƥ����
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = RTC_Hours;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = RTC_Minutes;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = RTC_Seconds;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm, &RTC_AlarmStruct);
	
	// �������ӣ����ȴ� RTC �Ĵ����ɷ���
	RTC_AlarmCmd(RTC_Alarm, ENABLE);
	// ������ӱ�־
	if (RTC_Alarm == RTC_Alarm_A)  // ���� A
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);
	}
	else   // ���� B    
	{
		RTC_ClearFlag(RTC_FLAG_ALRBF);
	}
	
}

/**
 * ���ܣ�  �������� RTC_Alarm �ж�
 * ������  RTC_Alarm ����ѡ�񡣿�ȡֵ��
 *             RTC_Alarm_A ���� A
 *             RTC_Alarm_B ���� B
 * ����ֵ����
 **/
void rtc_set_alarm_int(uint32_t RTC_Alarm)
{
	/*
	Ҫʹ�� RTC �����жϣ��谴������˳�������
	1. �� EXTI �� 17 ����Ϊ�ж�ģʽ������ʹ�ܣ�Ȼ��ѡ����������Ч��
	2. ���� NVIC �е� RTC_Alarm IRQ ͨ��������ʹ�ܡ�
	3. ���� RTC ������ RTC ����
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	// ������� A ���жϱ�־
	RTC_ClearITPendingBit(RTC_IT_ALRA);������ A ������ B����
	 */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// ��� EXTI 17 �ж����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	// ���� EXTI 17 �ж�
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	// ���� NVIC 
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	if (RTC_Alarm == RTC_Alarm_A)  // ���� A
	{
		// ʹ�� RTC �����ж�
		RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		// ������� A ���жϱ�־
		RTC_ClearITPendingBit(RTC_IT_ALRA);
	}
	else   // ���� B
	{
		// ʹ�� RTC �����ж�
		RTC_ITConfig(RTC_IT_ALRB, ENABLE);
		// ������� A ���жϱ�־
		RTC_ClearITPendingBit(RTC_IT_ALRB);
	}
}

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
void rtc_set_wakeup(uint32_t RTC_WakeUpCounter, uint32_t RTC_WakeUpClock)
{
	/*
	��̻��Ѷ�ʱ��
	Ҫ���û���Ļ��Ѷ�ʱ�����Զ�����ֵ��RTC_WUTR �е� WUT[15:0]������Ҫ��������˳�������
	1. ���� RTC_CR �е� WUTE �Խ�ֹ���Ѷ�ʱ����
	2. ��ѯ RTC_ISR �е� WUTWF��ֱ����λ�� 1����ȷ�����Է��ʻ����Զ�����
	   ��ʱ���� WUCKSEL[2:0] λ����Լ��Ҫ 2 �� RTCCLK ʱ�����ڣ�����ʱ��ͬ������
	3. ��̻����Զ�����ֵ WUT[15:0]����ѡ����ʱ�ӣ�RTC_CR �е� WUCKSEL[2:0] λ����
	   �� RTC_CR �Ĵ����е� WUTE λ�� 1 ���ٴ�ʹ�ܶ�ʱ�������Ѷ�ʱ�����¿�ʼ�ݼ�
	   ������
	 */
	//void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);
	//void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);
	//uint32_t RTC_GetWakeUpCounter(void);
	//ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);
	// ��ֹ���Ѷ�ʱ��
	RTC_WakeUpCmd(DISABLE);
	
	// ���û����Զ�����ֵ
	RTC_SetWakeUpCounter(RTC_WakeUpCounter);
	
	// ѡ����ʱ��
	RTC_WakeUpClockConfig(RTC_WakeUpClock);
	
	// ʹ�ܻ��Ѷ�ʱ��
	RTC_WakeUpCmd(ENABLE);
	
	// ������ѱ�־
	RTC_ClearFlag(RTC_FLAG_WUTF);
}

/**
 * ���ܣ�  ���û����ж�
 * ������  ��
 * ����ֵ����
 **/
void rtc_set_wakeup_int(void)
{
	/*
	Ҫʹ�� RTC �����жϣ��谴������˳�������
	1. �� EXTI �� 22 ����Ϊ�ж�ģʽ������ʹ�ܣ�Ȼ��ѡ����������Ч��
	2. ���� NVIC �е� RTC_WKUP IRQ ͨ��������ʹ�ܡ�
	3. ���� RTC ������ RTC ���Ѷ�ʱ���¼���
	 */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// ��� EXTI 22 �ж����жϱ�־
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	// ���� EXTI 22 �ж���
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	// ���� NVIC
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	// ʹ�� RTC �����ж�
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	// ������� A ���жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
}

static volatile uint8_t alarma_flag = 0;
static volatile uint8_t alarmb_flag = 0;
static volatile uint8_t wakeup_flag = 0;

/**
 * ���ܣ�  �������� A �Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_alarma(void)
{
	uint8_t flag = alarma_flag;
	alarma_flag = 0;
	return flag;
}

/**
 * ���ܣ�  �������� B �Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_alarmb(void)
{
	uint8_t flag = alarmb_flag;
	alarmb_flag = 0;
	return flag;
}

/**
 * ���ܣ�  ���ػ����Ƿ��ڣ���������ڱ�־
 * ������  ��
 * ����ֵ�����ڷ��� 1��δ���ڷ��� 0
 **/
uint8_t rtc_is_wakeup(void)
{
	uint8_t flag = wakeup_flag;
	wakeup_flag = 0;
	return flag;
}



/**
 * ���ܣ�  �����жϴ�����
 * ������  ��
 * ����ֵ����
 **/
void RTC_Alarm_IRQHandler(void)
{
	if (SET == RTC_GetITStatus(RTC_IT_ALRA))
	{
		alarma_flag = 1;
		// ������� A ��־
		RTC_ClearITPendingBit(RTC_IT_ALRA);
	}
	
	if (SET == RTC_GetITStatus(RTC_IT_ALRB))
	{
		alarmb_flag = 1;
		// ������� A ��־
		RTC_ClearITPendingBit(RTC_IT_ALRB);
	}
	// ����ж��� 17 ��־
	EXTI_ClearITPendingBit(EXTI_Line17);
	LED0_ON();
	LED3_ON();
}

/**
 * ���ܣ�  �����жϴ�����
 * ������  ��
 * ����ֵ����
 **/
void RTC_WKUP_IRQHandler(void)
{
	wakeup_flag = 1;
	// ������� A ���жϱ�־
	RTC_ClearITPendingBit(RTC_IT_WUT);
	// ����ж��� 22 ��־
	EXTI_ClearITPendingBit(EXTI_Line22);
}
