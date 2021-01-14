
#include "stm32f4xx.h"
#include "iwdg.h"

// 注意，当通过对关键字寄存器 (IWDG_KR) 写入值 0xCCCC 启动独立看门狗时，计数器开始从复位
// 值 0xFFF 递减计数。
// 任何时候将关键字 0xAAAA 写到 IWWDG_KR 寄存器中， IWDG_RLR 的值就会被重载到计数器，从而
// 避免产生看门狗复位。
// 因此，初始化看门狗后，如果不喂狗，就是从 0x0FFF 开始递减计数，只有喂狗了才使用设置的
// 重载计数值递减计数

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
void iwdg_init(uint8_t IWDG_Prescaler, uint16_t Reload)
{
	// 取消对 IWDG_PR 和 IWDG_RLR 寄存器的写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	// 配置预分频值
	IWDG_SetPrescaler(IWDG_Prescaler);
	
	// 设置重装载值
	IWDG_SetReload(Reload);
	
	// 使能看门狗
	IWDG_Enable();
	
	// 使能对 IWDG_PR 和 IWDG_RLR 寄存器的写保护
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Disable);
	
	// 喂狗一次，之后才会使用重载计数值开始递减计数
	IWDG_FEED();
}


