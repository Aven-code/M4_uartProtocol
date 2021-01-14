
#include "stm32f4xx.h"
#include "led.h"

/*
	查原理图可知：

	LED0     -> PF9
	LED1     -> PF10
	FSMC_D10 -> PE13
	PSMC_D11 -> PE14

	对这些引脚输出低电平（即 GPIOx_ODR 寄存器相应位清 0）则 LED 亮，
	输出高电平（即 GPIOx_ODR 寄存器相应位置 1）则 LED 灭

 */


#ifdef LED_USE_LIBRARY    // 使用库函数方式

/**
 * 功能：  初始化 LED(使用库函数方式)
 * 参数：  无
 * 返回值：无
 **/
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// 使能 GPIOE 和 GPIOF 的时钟
	// void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE 
		| RCC_AHB1Periph_GPIOF,	ENABLE);
	
	// 初始化 GPIOE 和 GPIOF
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    // 输出
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   // 推挽
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;     // 上拉
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz; // 100MHz
	// void GPIO_Init(GPIO_TypeDef* GPIOx, GPIO_InitTypeDef* GPIO_InitStruct);
	GPIO_Init(GPIOE, &GPIO_InitStruct);
	
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_Init(GPIOF, &GPIO_InitStruct);
	
	LED0_OFF();
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
}

#elif defined(LED_USE_STRUCTURE)   // 使用结构体方式

#if 0

// 以下是从库文件中复制的内容，帮助理解结构体方式访问寄存器
// ------------------------------------------------------------------------------------------------
typedef struct
{
  __IO uint32_t MODER;    /*!< GPIO port mode register,               Address offset: 0x00      */
  __IO uint32_t OTYPER;   /*!< GPIO port output type register,        Address offset: 0x04      */
  __IO uint32_t OSPEEDR;  /*!< GPIO port output speed register,       Address offset: 0x08      */
  __IO uint32_t PUPDR;    /*!< GPIO port pull-up/pull-down register,  Address offset: 0x0C      */
  __IO uint32_t IDR;      /*!< GPIO port input data register,         Address offset: 0x10      */
  __IO uint32_t ODR;      /*!< GPIO port output data register,        Address offset: 0x14      */
  __IO uint16_t BSRRL;    /*!< GPIO port bit set/reset low register,  Address offset: 0x18      */
  __IO uint16_t BSRRH;    /*!< GPIO port bit set/reset high register, Address offset: 0x1A      */
  __IO uint32_t LCKR;     /*!< GPIO port configuration lock register, Address offset: 0x1C      */
  __IO uint32_t AFR[2];   /*!< GPIO alternate function registers,     Address offset: 0x20-0x24 */
} GPIO_TypeDef;

typedef struct
{
  __IO uint32_t CR;            /*!< RCC clock control register,                                  Address offset: 0x00 */
  __IO uint32_t PLLCFGR;       /*!< RCC PLL configuration register,                              Address offset: 0x04 */
  __IO uint32_t CFGR;          /*!< RCC clock configuration register,                            Address offset: 0x08 */
  __IO uint32_t CIR;           /*!< RCC clock interrupt register,                                Address offset: 0x0C */
  __IO uint32_t AHB1RSTR;      /*!< RCC AHB1 peripheral reset register,                          Address offset: 0x10 */
  __IO uint32_t AHB2RSTR;      /*!< RCC AHB2 peripheral reset register,                          Address offset: 0x14 */
  __IO uint32_t AHB3RSTR;      /*!< RCC AHB3 peripheral reset register,                          Address offset: 0x18 */
  uint32_t      RESERVED0;     /*!< Reserved, 0x1C                                                                    */
  __IO uint32_t APB1RSTR;      /*!< RCC APB1 peripheral reset register,                          Address offset: 0x20 */
  __IO uint32_t APB2RSTR;      /*!< RCC APB2 peripheral reset register,                          Address offset: 0x24 */
  uint32_t      RESERVED1[2];  /*!< Reserved, 0x28-0x2C                                                               */
  __IO uint32_t AHB1ENR;       /*!< RCC AHB1 peripheral clock register,                          Address offset: 0x30 */
  __IO uint32_t AHB2ENR;       /*!< RCC AHB2 peripheral clock register,                          Address offset: 0x34 */
  __IO uint32_t AHB3ENR;       /*!< RCC AHB3 peripheral clock register,                          Address offset: 0x38 */
  uint32_t      RESERVED2;     /*!< Reserved, 0x3C                                                                    */
  __IO uint32_t APB1ENR;       /*!< RCC APB1 peripheral clock enable register,                   Address offset: 0x40 */
  __IO uint32_t APB2ENR;       /*!< RCC APB2 peripheral clock enable register,                   Address offset: 0x44 */
  uint32_t      RESERVED3[2];  /*!< Reserved, 0x48-0x4C                                                               */
  __IO uint32_t AHB1LPENR;     /*!< RCC AHB1 peripheral clock enable in low power mode register, Address offset: 0x50 */
  __IO uint32_t AHB2LPENR;     /*!< RCC AHB2 peripheral clock enable in low power mode register, Address offset: 0x54 */
  __IO uint32_t AHB3LPENR;     /*!< RCC AHB3 peripheral clock enable in low power mode register, Address offset: 0x58 */
  uint32_t      RESERVED4;     /*!< Reserved, 0x5C                                                                    */
  __IO uint32_t APB1LPENR;     /*!< RCC APB1 peripheral clock enable in low power mode register, Address offset: 0x60 */
  __IO uint32_t APB2LPENR;     /*!< RCC APB2 peripheral clock enable in low power mode register, Address offset: 0x64 */
  uint32_t      RESERVED5[2];  /*!< Reserved, 0x68-0x6C                                                               */
  __IO uint32_t BDCR;          /*!< RCC Backup domain control register,                          Address offset: 0x70 */
  __IO uint32_t CSR;           /*!< RCC clock control & status register,                         Address offset: 0x74 */
  uint32_t      RESERVED6[2];  /*!< Reserved, 0x78-0x7C                                                               */
  __IO uint32_t SSCGR;         /*!< RCC spread spectrum clock generation register,               Address offset: 0x80 */
  __IO uint32_t PLLI2SCFGR;    /*!< RCC PLLI2S configuration register,                           Address offset: 0x84 */
  __IO uint32_t PLLSAICFGR;    /*!< RCC PLLSAI configuration register,                           Address offset: 0x88 */
  __IO uint32_t DCKCFGR;       /*!< RCC Dedicated Clocks configuration register,                 Address offset: 0x8C */
  __IO uint32_t CKGATENR;      /*!< RCC Clocks Gated Enable Register,                            Address offset: 0x90 */ /* Only for STM32F412xG, STM32413_423xx and STM32F446xx devices */
  __IO uint32_t DCKCFGR2;      /*!< RCC Dedicated Clocks configuration register 2,               Address offset: 0x94 */ /* Only for STM32F410xx, STM32F412xG, STM32413_423xx and STM32F446xx devices */

} RCC_TypeDef;

#define PERIPH_BASE           ((uint32_t)0x40000000)
	
#define AHB1PERIPH_BASE       (PERIPH_BASE + 0x00020000)
	
#define GPIOA_BASE            (AHB1PERIPH_BASE + 0x0000)
#define GPIOB_BASE            (AHB1PERIPH_BASE + 0x0400)
#define GPIOC_BASE            (AHB1PERIPH_BASE + 0x0800)
#define GPIOD_BASE            (AHB1PERIPH_BASE + 0x0C00)
#define GPIOE_BASE            (AHB1PERIPH_BASE + 0x1000)    // 0x40021000
#define GPIOF_BASE            (AHB1PERIPH_BASE + 0x1400)    // 0x40021400
	
#define RCC_BASE              (AHB1PERIPH_BASE + 0x3800)
	
#define GPIOA               ((GPIO_TypeDef *) GPIOA_BASE)
#define GPIOB               ((GPIO_TypeDef *) GPIOB_BASE)
#define GPIOC               ((GPIO_TypeDef *) GPIOC_BASE)
#define GPIOD               ((GPIO_TypeDef *) GPIOD_BASE)
#define GPIOE               ((GPIO_TypeDef *) GPIOE_BASE)
#define GPIOF               ((GPIO_TypeDef *) GPIOF_BASE)
	
#define RCC                 ((RCC_TypeDef *) RCC_BASE)

// ------------------------------------------------------------------------------------------------
#endif

/**
 * 功能：  初始化 LED - 使用结构体方式
 * 参数：  无
 * 返回值：无
 **/
void led_init(void)
{
	// 要使 GPIOE 和 GPIOF 工作，就要使能它们的时钟
	// 在 STM32F4 的中文手册的 "2 存储器和总线架构" 之 "2.3 存储器映射" 中 "表 2. STM32F4xx 寄存器边界地址"
	// 可查到所有外设所属总线，也可以查到外设的边界地址
	// 经查，GPIOE 和 GPIOF 都属于 AHB1 总线
	// 在 "6.3.12 RCC AHB1 外设时钟使能寄存器 (RCC_AHB1ENR)" 中可查到，GPIOE 和 GPIOF 的时钟分别由 [4] 和 [5] 位
	// 控制：1 使能，0 禁用。其偏移地址为 0x30。
	// 每个寄存器都有一个地址，在 "表 2. STM32F4xx 寄存器边界地址" 可以查到 RCC 的起始地址为 0x4002 3800 
	// 因此，得到 RCC_AHB1ENR 的地址为 0x40023800 + 0x30
	// 0x3 << 4 和 (1 << 4) | (1 << 5) 最终得到的值都是 0b110000
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= 0x3 << 4;   // [4] 和 [5] 位置 1
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= (1 << 4) | (1 << 5);
	RCC->AHB1ENR |= 0x3 << 4;
	
	// 配置 PF9, PF10
	// 经查 GPIOF 的起始地址为 0x4002 1400 
	// 7.4.1 GPIO 端口模式寄存器 (GPIOx_MODER) (x = A..I) 偏移地址 0x00
	// 模式有：00：输入（复位状态）；01：通用输出模式；10：复用功能模式；11：模拟模式
	// LED 是 "通用输出模式"，即 01
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	//*((volatile unsigned int *) (0x40021400 + 0x00)) &= ~(0xF << 18);   // 4 个位清 0
	//*((volatile unsigned int *) (0x40021400 + 0x00)) |= 0x5 << 18;      // 4 个位赋值为 0b0101
	GPIOF->MODER &= ~(0xF << 18);
	GPIOF->MODER |= 0x5 << 18;
	
	// 7.4.2 GPIO 端口输出类型寄存器 (GPIOx_OTYPER) (x = A..I) 偏移地址 0x04
	// 输出类型有：0：输出推挽（复位状态）；1：输出开漏
	// LED 输出的是数字信号，因此选择输出推挽，即 0
	// PF9 和 PF10 分别占 [9] 位和 [10] 位
	//*((volatile unsigned int *) (0x40021400 + 0x04)) &= ~(0x3 << 9);    // 2 个位清 0
	GPIOF->OTYPER &= ~(0x3 << 9);

	// 7.4.3 GPIO 端口输出速度寄存器 (GPIOx_OSPEEDR) (x = A..I/) 偏移地址 0x08
	// 输出速度有：00： 2 MHz（低速）；01： 25 MHz（中速）；10： 50 MHz（快速）
	//             11： 30 pF 时为 100 MHz（高速）（15 pF 时为 80 MHz 输出（最大速度））
	// LED 输出速度可任选，这里选择高速，即 11
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	//*((volatile unsigned int *) (0x40021400 + 0x08)) |= 0xF << 18;      // 4 个位置 1
	GPIOF->OSPEEDR |= 0xF << 18;
	
	// 7.4.4 GPIO 端口上拉/下拉寄存器 (GPIOx_PUPDR) (x = A..I/) 偏移地址 0x0C
	// 上拉/下拉有 4 种选择：00：无上拉或下拉；01：上拉；10：下拉；11：保留
	// LED 可以选择上拉，即 01
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	//*((volatile unsigned int *) (0x40021400 + 0x0C)) &= ~(0xF << 18);   // 4 个位清 0
	//*((volatile unsigned int *) (0x40021400 + 0x0C)) |= 0x5 << 18;      // 4 个位赋值为 0b0101
	GPIOF->PUPDR &= ~(0xF << 18);
	GPIOF->PUPDR |= 0x5 << 18;
	
	// 配置 PE13, PE14
	// 经查 GPIOE 的起始地址为 0x4002 1000
	// 模式寄存器 (GPIOx_MODER)
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	//*((volatile unsigned int *) (0x40021000 + 0x00)) &= ~(0xF << 26);   // 4 个位清 0
	//*((volatile unsigned int *) (0x40021000 + 0x00)) |= 0x5 << 26;      // 4 个位赋值为 0b0101
	GPIOE->MODER &= ~(0xF << 26);
	GPIOE->MODER |= 0x5 << 26;
	
	// 输出类型寄存器 (GPIOx_OTYPER)
	// PE13 和 PE14 分别占 [13] 位和 [14] 位
	//*((volatile unsigned int *) (0x40021000 + 0x04)) &= ~(0x3 << 13);    // 2 个位清 0
	GPIOE->OTYPER &= ~(0x3 << 13);
	
	// 输出速度寄存器 (GPIOx_OSPEEDR)
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	//*((volatile unsigned int *) (0x40021000 + 0x08)) |= 0xF << 26;      // 4 个位置 1
	GPIOE->OSPEEDR |= 0xF << 26;
	
	// 上拉/下拉寄存器 (GPIOx_PUPDR) 
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	//*((volatile unsigned int *) (0x40021000 + 0x0C)) &= ~(0xF << 26);   // 4 个位清 0
	//*((volatile unsigned int *) (0x40021000 + 0x0C)) |= 0x5 << 26;      // 4 个位赋值为 0b0101
	GPIOE->PUPDR &= ~(0xF << 26);
	GPIOE->PUPDR |= 0x5 << 26;
	
	
	LED0_OFF();
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
		
}

#elif defined(LED_USE_ORIGINAL)     // 使用原生方式    

/**
 * 功能：  初始化 LED - 原生方式
 * 参数：  无
 * 返回值：无
 **/
void led_init(void)
{
	// 要使 GPIOE 和 GPIOF 工作，就要使能它们的时钟
	// 在 STM32F4 的中文手册的 "2 存储器和总线架构" 之 "2.3 存储器映射" 中 "表 2. STM32F4xx 寄存器边界地址"
	// 可查到所有外设所属总线，也可以查到外设的边界地址
	// 经查，GPIOE 和 GPIOF 都属于 AHB1 总线
	// 在 "6.3.12 RCC AHB1 外设时钟使能寄存器 (RCC_AHB1ENR)" 中可查到，GPIOE 和 GPIOF 的时钟分别由 [4] 和 [5] 位
	// 控制：1 使能，0 禁用。其偏移地址为 0x30。
	// 每个寄存器都有一个地址，在 "表 2. STM32F4xx 寄存器边界地址" 可以查到 RCC 的起始地址为 0x4002 3800 
	// 因此，得到 RCC_AHB1ENR 的地址为 0x40023800 + 0x30
	// 0x3 << 4 和 (1 << 4) | (1 << 5) 最终得到的值都是 0b110000
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= 0x3 << 4;   // [4] 和 [5] 位置 1
	*((volatile unsigned int *) (0x40023800 + 0x30)) |= (1 << 4) | (1 << 5);

	// 配置 PF9, PF10
	// 经查 GPIOF 的起始地址为 0x4002 1400 
	// 7.4.1 GPIO 端口模式寄存器 (GPIOx_MODER) (x = A..I) 偏移地址 0x00
	// 模式有：00：输入（复位状态）；01：通用输出模式；10：复用功能模式；11：模拟模式
	// LED 是 "通用输出模式"，即 01
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	*((volatile unsigned int *) (0x40021400 + 0x00)) &= ~(0xF << 18);   // 4 个位清 0
	*((volatile unsigned int *) (0x40021400 + 0x00)) |= 0x5 << 18;      // 4 个位赋值为 0b0101

	// 7.4.2 GPIO 端口输出类型寄存器 (GPIOx_OTYPER) (x = A..I) 偏移地址 0x04
	// 输出类型有：0：输出推挽（复位状态）；1：输出开漏
	// LED 输出的是数字信号，因此选择输出推挽，即 0
	// PF9 和 PF10 分别占 [9] 位和 [10] 位
	*((volatile unsigned int *) (0x40021400 + 0x04)) &= ~(0x3 << 9);    // 2 个位清 0

	// 7.4.3 GPIO 端口输出速度寄存器 (GPIOx_OSPEEDR) (x = A..I/) 偏移地址 0x08
	// 输出速度有：00： 2 MHz（低速）；01： 25 MHz（中速）；10： 50 MHz（快速）
	//             11： 30 pF 时为 100 MHz（高速）（15 pF 时为 80 MHz 输出（最大速度））
	// LED 输出速度可任选，这里选择高速，即 11
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	*((volatile unsigned int *) (0x40021400 + 0x08)) |= 0xF << 18;      // 4 个位置 1
	
	// 7.4.4 GPIO 端口上拉/下拉寄存器 (GPIOx_PUPDR) (x = A..I/) 偏移地址 0x0C
	// 上拉/下拉有 4 种选择：00：无上拉或下拉；01：上拉；10：下拉；11：保留
	// LED 可以选择上拉，即 01
	// PF9 和 PF10 分别占 [19:18] 位和 [21:20] 位
	*((volatile unsigned int *) (0x40021400 + 0x0C)) &= ~(0xF << 18);   // 4 个位清 0
	*((volatile unsigned int *) (0x40021400 + 0x0C)) |= 0x5 << 18;      // 4 个位赋值为 0b0101
	
	// 配置 PE13, PE14
	// 经查 GPIOE 的起始地址为 0x4002 1000
	// 模式寄存器 (GPIOx_MODER)
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	*((volatile unsigned int *) (0x40021000 + 0x00)) &= ~(0xF << 26);   // 4 个位清 0
	*((volatile unsigned int *) (0x40021000 + 0x00)) |= 0x5 << 26;      // 4 个位赋值为 0b0101

	// 输出类型寄存器 (GPIOx_OTYPER)
	// PE13 和 PE14 分别占 [13] 位和 [14] 位
	*((volatile unsigned int *) (0x40021000 + 0x04)) &= ~(0x3 << 13);    // 2 个位清 0

	// 输出速度寄存器 (GPIOx_OSPEEDR)
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	*((volatile unsigned int *) (0x40021000 + 0x08)) |= 0xF << 26;      // 4 个位置 1

	// 上拉/下拉寄存器 (GPIOx_PUPDR) 
	// PE13 和 PE14 分别占 [27:26] 位和 [29:28] 位
	*((volatile unsigned int *) (0x40021000 + 0x0C)) &= ~(0xF << 26);   // 4 个位清 0
	*((volatile unsigned int *) (0x40021000 + 0x0C)) |= 0x5 << 26;      // 4 个位赋值为 0b0101
}

#endif

