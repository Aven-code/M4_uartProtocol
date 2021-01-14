
#include "rtc.h"
#include "stm32f4xx.h"
#include "stdio.h"
#include "led.h"

// RTC 中断标志清除注意：
// 1. 闹钟是连接到 EXTI 17 线，因此既要清除 RTC 闹钟中断标志，
//    又要清除 EXTI 17 的中断标志
//    同理，唤醒是连接到 EXTI 22 线，因此既要清除 RTC 唤醒中断标志，
//    又要清除 EXTI 22 的中断标志
// 清除 EXTI 17 中断线中断标志: EXTI_ClearITPendingBit(EXTI_Line17);
// 清除闹钟 A 的中断标志:       RTC_ClearITPendingBit(RTC_IT_ALRA);
// 清除 EXTI 22 中断线中断标志: EXTI_ClearITPendingBit(EXTI_Line22);
// 清除唤醒的中断标志:          RTC_ClearITPendingBit(RTC_IT_WUT);
// 以上标志在初始化时，以及在中断处理中都要清除

// 2. 闹钟和唤醒标志的清除：
//    在不使用中断的情况下，闹钟和唤醒也存在标志位，它们也需要清除
// 清除闹钟 A 标志: RTC_ClearFlag(RTC_FLAG_ALRAF);
// 清除唤醒标志:    RTC_ClearFlag(RTC_FLAG_WUTF);

// 等待 LSE 就绪超时值
#define LSE_READY_TIMEOUT  0xFFFFFFUL
// 保存 RTC 已初始化过的标志寄存器
#define RTC_INIT_REG RTC_BKP_DR0
// RTC 已初始化标志
#define RTC_INITIALIZED_FLAG 0x87654321

/**
 * 功能：  初始化实时时钟
 * 参数：  无
 * 返回值：成功返回 SUCCESS，失败返回 ERROR
 **/
ErrorStatus rtc_init(void)
{
	uint32_t count = 0;
	RTC_InitTypeDef RTC_InitStruct;
	
	// 使能 PWR 外设时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
	// void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
	// void        RCC_RTCCLKCmd(FunctionalState NewState);
	// 使能备份域访问
	// void PWR_BackupAccessCmd(FunctionalState NewState);
	PWR_BackupAccessCmd(ENABLE);
	
	// 使能 RTC 外设时钟
	RCC_RTCCLKCmd(ENABLE);
	
	// 判断之前是否初始化过了，如果已初始化过了，则直接返回
	if (RTC_INITIALIZED_FLAG == RTC_ReadBackupRegister(RTC_INIT_REG))
	{
		return SUCCESS;
	}
	
	// 开启 LSE 时钟源
	// void        RCC_LSEConfig(uint8_t RCC_LSE);
	RCC_LSEConfig(RCC_LSE_ON);
	
	// 等待时钟源就绪，RCC->BDCR 的 [1] 位变为 1
	while (!(RCC->BDCR & 0x2) && (count < LSE_READY_TIMEOUT))
	{
		count++;
	}
	if (count == LSE_READY_TIMEOUT)  // 超时
	{
			return ERROR;
	}
	
	// 选择 LSE 作为 RTC 时钟源
	// void        RCC_RTCCLKConfig(uint32_t RCC_RTCCLKSource);
	RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);
	
	// 初始化 RTC
	RTC_InitStruct.RTC_AsynchPrediv = 0x7F;
	RTC_InitStruct.RTC_SynchPrediv = 0xFF;
	RTC_InitStruct.RTC_HourFormat = RTC_HourFormat_24;
	// ErrorStatus RTC_Init(RTC_InitTypeDef* RTC_InitStruct);
	RTC_Init(&RTC_InitStruct);
	
	// 写入已初始化标志到备份域寄存器
	// void RTC_WriteBackupRegister(uint32_t RTC_BKP_DR, uint32_t Data);
	// 解锁
	RTC->WPR = 0xCA;
  RTC->WPR = 0x53;
	RTC_WriteBackupRegister(RTC_INIT_REG, RTC_INITIALIZED_FLAG);
	// 加锁
	RTC->WPR = 0xFF;
	
	return SUCCESS;
}

/**
 * 功能：  设置 RTC 时间，默认 24 小时制
 * 参数：  hour 时
 *         min  分
 *         sec  秒
 * 返回值：成功返回 SUCCESS，失败返回 ERROR
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
 * 功能：  打印当前日期时间
 * 参数：  无
 * 返回值：无
 **/
void rtc_print_date_time(void)
{
	RTC_DateTypeDef date;
	RTC_TimeTypeDef time;
	
	/*
		中文手册 《23.3.6 读取日历》：
		当 RTC_CR 寄存器中的 BYPSHAD 控制位清零时，即从影子寄存器读取日历时，
		每次将日历寄存器中的值复制到 RTC_SSR、 RTC_TR 和 RTC_DR 影子寄存器时， 
		RTC_ISR 寄存器中的 RSF 位都会置 1。每两个 TRCCLK 周期执行一次复制。为
		确保这 3 个值来自同一时刻点，读取 RTC_SSR 或 RTC_TR 时会锁定高阶日历
		影子寄存器中的值，直到读取 RTC_DR。
		为避免软件对日历执行读访问的时间间隔小于 2 个 RTCCLK 周期：第一次读取
		日历之后必须通过软件将 RSF 清零，并且软件必须等待到 RSF 置 1 之后才可
		再次读取 RTC_SSR、 RTC_TR 和 RTC_DR	寄存器。
		ErrorStatus RTC_WaitForSynchro(void) 函数就是清零 RSF 位并等待其变 1 的
	 */
	
	// 读取日期时间时，必须先读取时间再读日期，否则结果不对
	RTC_GetTime(RTC_Format_BIN, &time);
	RTC_GetDate(RTC_Format_BIN, &date);
	
	RTC_WaitForSynchro();
	
	printf("%04d-%02d-%02d %02d:%02d:%02d %d", 
		date.RTC_Year + 2000, date.RTC_Month, date.RTC_Date, 
		time.RTC_Hours, time.RTC_Minutes, time.RTC_Seconds,
		date.RTC_WeekDay);
}



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
	uint8_t RTC_Hours, uint8_t RTC_Minutes, uint8_t RTC_Seconds)
{
	/*
	编程闹钟
  要对可编程的闹钟（闹钟 A 或闹钟 B）进行编程或更新，必须执行类似的步骤：
  1. 将 RTC_CR 寄存器中的 ALRAE 或 ALRBE 位清零以禁止闹钟 A 或闹钟 B。
  2. 轮询 RTC_ISR 寄存器中的 ALRAWF 或 ALRBWF 位，直到其中一个置 1，以确保
	   闹钟寄存器可以访问。大约需要 2 个 RTCCLK 时钟周期（由于时钟同步）。
  3. 编程闹钟 A 或闹钟 B 寄存器（RTC_ALRMASSR/RTC_ALRMAR 或 RTC_ALRMBSSR/
	   RTC_ALRMBR）。
  4. 将 RTC_CR 寄存器中的 ALRAE 或 ALRBE 位置 1 以再次使能闹钟 A 或闹钟 B。
	 */
	RTC_AlarmTypeDef RTC_AlarmStruct;
	
	// 禁用闹钟，并等待 RTC 寄存器可访问
	RTC_AlarmCmd(RTC_Alarm, DISABLE);
	
	// 设置闹钟
	RTC_AlarmStruct.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel;
	RTC_AlarmStruct.RTC_AlarmDateWeekDay = RTC_AlarmDateWeekDay;
	// 闹钟掩码，指按日期/星期闹，还是小时、分钟、秒等闹，也可以全部匹配闹
	// RTC_AlarmMask_None 全部匹配，RTC_AlarmMask_All 全部无关
	RTC_AlarmStruct.RTC_AlarmMask = RTC_AlarmMask_None;   // 全部匹配闹
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Hours = RTC_Hours;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Minutes = RTC_Minutes;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_Seconds = RTC_Seconds;
	RTC_AlarmStruct.RTC_AlarmTime.RTC_H12 = RTC_H12_AM;
	RTC_SetAlarm(RTC_Format_BIN, RTC_Alarm, &RTC_AlarmStruct);
	
	// 启用闹钟，并等待 RTC 寄存器可访问
	RTC_AlarmCmd(RTC_Alarm, ENABLE);
	// 清除闹钟标志
	if (RTC_Alarm == RTC_Alarm_A)  // 闹钟 A
	{
		RTC_ClearFlag(RTC_FLAG_ALRAF);
	}
	else   // 闹钟 B    
	{
		RTC_ClearFlag(RTC_FLAG_ALRBF);
	}
	
}

/**
 * 功能：  设置闹钟 RTC_Alarm 中断
 * 参数：  RTC_Alarm 闹钟选择。可取值：
 *             RTC_Alarm_A 闹钟 A
 *             RTC_Alarm_B 闹钟 B
 * 返回值：无
 **/
void rtc_set_alarm_int(uint32_t RTC_Alarm)
{
	/*
	要使能 RTC 闹钟中断，需按照以下顺序操作：
	1. 将 EXTI 线 17 配置为中断模式并将其使能，然后选择上升沿有效。
	2. 配置 NVIC 中的 RTC_Alarm IRQ 通道并将其使能。
	3. 配置 RTC 以生成 RTC 闹钟
	
	RTC_ITConfig(RTC_IT_ALRA, ENABLE);
	// 清除闹钟 A 的中断标志
	RTC_ClearITPendingBit(RTC_IT_ALRA);（闹钟 A 或闹钟 B）。
	 */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 清除 EXTI 17 中断线中断标志
	EXTI_ClearITPendingBit(EXTI_Line17);
	
	// 配置 EXTI 17 中断
	EXTI_InitStruct.EXTI_Line = EXTI_Line17;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	// 配置 NVIC 
	NVIC_InitStruct.NVIC_IRQChannel = RTC_Alarm_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 2;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStruct);

	if (RTC_Alarm == RTC_Alarm_A)  // 闹钟 A
	{
		// 使能 RTC 闹钟中断
		RTC_ITConfig(RTC_IT_ALRA, ENABLE);
		// 清除闹钟 A 的中断标志
		RTC_ClearITPendingBit(RTC_IT_ALRA);
	}
	else   // 闹钟 B
	{
		// 使能 RTC 闹钟中断
		RTC_ITConfig(RTC_IT_ALRB, ENABLE);
		// 清除闹钟 A 的中断标志
		RTC_ClearITPendingBit(RTC_IT_ALRB);
	}
}

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
void rtc_set_wakeup(uint32_t RTC_WakeUpCounter, uint32_t RTC_WakeUpClock)
{
	/*
	编程唤醒定时器
	要配置或更改唤醒定时器的自动重载值（RTC_WUTR 中的 WUT[15:0]），需要按照以下顺序操作：
	1. 清零 RTC_CR 中的 WUTE 以禁止唤醒定时器。
	2. 轮询 RTC_ISR 中的 WUTWF，直到该位置 1，以确保可以访问唤醒自动重载
	   定时器和 WUCKSEL[2:0] 位。大约需要 2 个 RTCCLK 时钟周期（由于时钟同步）。
	3. 编程唤醒自动重载值 WUT[15:0]，并选择唤醒时钟（RTC_CR 中的 WUCKSEL[2:0] 位）。
	   将 RTC_CR 寄存器中的 WUTE 位置 1 以再次使能定时器。唤醒定时器重新开始递减
	   计数。
	 */
	//void RTC_WakeUpClockConfig(uint32_t RTC_WakeUpClock);
	//void RTC_SetWakeUpCounter(uint32_t RTC_WakeUpCounter);
	//uint32_t RTC_GetWakeUpCounter(void);
	//ErrorStatus RTC_WakeUpCmd(FunctionalState NewState);
	// 禁止唤醒定时器
	RTC_WakeUpCmd(DISABLE);
	
	// 设置唤醒自动重载值
	RTC_SetWakeUpCounter(RTC_WakeUpCounter);
	
	// 选择唤醒时钟
	RTC_WakeUpClockConfig(RTC_WakeUpClock);
	
	// 使能唤醒定时器
	RTC_WakeUpCmd(ENABLE);
	
	// 清除唤醒标志
	RTC_ClearFlag(RTC_FLAG_WUTF);
}

/**
 * 功能：  设置唤醒中断
 * 参数：  无
 * 返回值：无
 **/
void rtc_set_wakeup_int(void)
{
	/*
	要使能 RTC 唤醒中断，需按照以下顺序操作：
	1. 将 EXTI 线 22 配置为中断模式并将其使能，然后选择上升沿有效。
	2. 配置 NVIC 中的 RTC_WKUP IRQ 通道并将其使能。
	3. 配置 RTC 以生成 RTC 唤醒定时器事件。
	 */
	EXTI_InitTypeDef EXTI_InitStruct;
	NVIC_InitTypeDef NVIC_InitStruct;
	
	// 清除 EXTI 22 中断线中断标志
	EXTI_ClearITPendingBit(EXTI_Line22);
	
	// 配置 EXTI 22 中断线
	EXTI_InitStruct.EXTI_Line = EXTI_Line22;
	EXTI_InitStruct.EXTI_LineCmd = ENABLE;
	EXTI_InitStruct.EXTI_Mode = EXTI_Mode_Interrupt;
	EXTI_InitStruct.EXTI_Trigger = EXTI_Trigger_Rising;
	EXTI_Init(&EXTI_InitStruct);
	
	// 配置 NVIC
	NVIC_InitStruct.NVIC_IRQChannel = RTC_WKUP_IRQn;
	NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
	NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 1;
	NVIC_InitStruct.NVIC_IRQChannelSubPriority = 3;
	NVIC_Init(&NVIC_InitStruct);
	
	// 使能 RTC 闹钟中断
	RTC_ITConfig(RTC_IT_WUT, ENABLE);
	// 清除闹钟 A 的中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
}

static volatile uint8_t alarma_flag = 0;
static volatile uint8_t alarmb_flag = 0;
static volatile uint8_t wakeup_flag = 0;

/**
 * 功能：  返回闹钟 A 是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_alarma(void)
{
	uint8_t flag = alarma_flag;
	alarma_flag = 0;
	return flag;
}

/**
 * 功能：  返回闹钟 B 是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_alarmb(void)
{
	uint8_t flag = alarmb_flag;
	alarmb_flag = 0;
	return flag;
}

/**
 * 功能：  返回唤醒是否到期，并清除到期标志
 * 参数：  无
 * 返回值：到期返回 1，未到期返回 0
 **/
uint8_t rtc_is_wakeup(void)
{
	uint8_t flag = wakeup_flag;
	wakeup_flag = 0;
	return flag;
}



/**
 * 功能：  闹钟中断处理函数
 * 参数：  无
 * 返回值：无
 **/
void RTC_Alarm_IRQHandler(void)
{
	if (SET == RTC_GetITStatus(RTC_IT_ALRA))
	{
		alarma_flag = 1;
		// 清除闹钟 A 标志
		RTC_ClearITPendingBit(RTC_IT_ALRA);
	}
	
	if (SET == RTC_GetITStatus(RTC_IT_ALRB))
	{
		alarmb_flag = 1;
		// 清除闹钟 A 标志
		RTC_ClearITPendingBit(RTC_IT_ALRB);
	}
	// 清除中断线 17 标志
	EXTI_ClearITPendingBit(EXTI_Line17);
	LED0_ON();
	LED3_ON();
}

/**
 * 功能：  唤醒中断处理函数
 * 参数：  无
 * 返回值：无
 **/
void RTC_WKUP_IRQHandler(void)
{
	wakeup_flag = 1;
	// 清除闹钟 A 的中断标志
	RTC_ClearITPendingBit(RTC_IT_WUT);
	// 清除中断线 22 标志
	EXTI_ClearITPendingBit(EXTI_Line22);
}
