/**
  ******************************************************************************
  * @file    Project/STM32F4xx_StdPeriph_Templates/main.c 
  * @author  MCD Application Team
  * @version V1.8.0
  * @date    04-November-2016
  * @brief   Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "led.h"
#include "mco.h"
#include "delay.h"
#include "bitbanding.h"
#include "beep.h"
#include "key.h"
#include "usart.h"
#include "stdio.h"
#include "data.h"

#include <stdlib.h>
#include <string.h>
#include "funcode.h"
 

//全局变量，Func_Code 功能码类型
Func_Code receiveMsgType[RECEV_MSGTYPE_MAX] = {
{0x1001, getStatusInfo}, {0x1002, setChannel}, {0x1003, setTotalVolume},
{0x1004, setResetBalance},{0x1005, setEqualizerRec},
{0x1006, setMute}, {0x1007, setPreSoundEffe}, {0x1008, systemReset},
{0x1009, setEqualizerCtr}, {0x100A, setGraphicEqualizer}, {0x100B, setJointCtr},
{0x100C, setDelayCtr}, 
{0x100D, reserve1}, {0x100E, reserve2}, {0x100F, reserve3}, 
{0x5001, sendEquipmentInfo}, 
{0x5007, BluetoothMatch}};

//全局参数属性
Stru_Data allConfig;
u8 receiveDataArray[RECEIVE_MAX] = {0};





/** @addtogroup Template_Project
  * @{
  */ 

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
RCC_ClocksTypeDef RCC_Clocks;

/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/

// 注：
//
// stm32f4xx.h 中的 144 行的
// #define HSE_VALUE    ((uint32_t)25000000)
// 改为
// #define HSE_VALUE    ((uint32_t)8000000)
//
// system_stm32f4xx.c 的 371 行的 
// #define PLL_M      25
// 改为
// #define PLL_M      8
//

/**
  * @brief  Main program
  * @param  None
  * @retval None
  */
int main(void)
{
	// 设置优先级分组为 2 组，即 抢占优先级位数 : 响应优先级位数 = 2 : 2
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
 /*!< At this stage the microcontroller clock setting is already configured, 
       this is done through SystemInit() function which is called from startup
       files before to branch to application main.
       To reconfigure the default setting of SystemInit() function, 
       refer to system_stm32f4xx.c file */

  /* SysTick end of count event each 10ms */
  RCC_GetClocksFreq(&RCC_Clocks);
	// 此函数配置 Systick 定时器每 10ms 产生一次中断
  SysTick_Config(RCC_Clocks.HCLK_Frequency / 100);
  
  /* Add your application code here */
  /* Insert 50 ms delay */
  Delay(5);
  
	// 初始化 MCO
  mco_init();  
  
	// 初始化 LED
	led_init();

	// 初始化 BEEP
	beep_init();
	
	// 初始化 USART1
	usart1_init(9600);
	//usart1_init(2400);
	//printf("%d\r\n", RCC_Clocks.HCLK_Frequency);
	
	StruInit(&allConfig); // 初始化
  initApp();

	/* Infinite loop */
  while (1)
  {
		//如果协议正确 全部灯全亮，否则全灭并且蜂鸣器响一声
		//流水灯循环点亮，中断执行特殊函数
		LED0_OFF();
		LED1_OFF();
		LED2_OFF();
		LED3_OFF();

		LED0_ON();
		LED1_OFF();
		LED2_OFF();
		LED3_OFF();
		Delay(50);
		LED0_OFF();
		LED1_ON();
  	Delay(50);
		LED1_OFF();
		LED2_ON();
		Delay(50);
		LED2_OFF();
		LED3_ON();
		Delay(50);
		LED3_OFF();
		LED0_ON();

		Delay(50);
	 
  }
}




 


#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

/**
  * @}
  */


/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
