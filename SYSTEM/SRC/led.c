
#include "stm32f4xx.h"
#include "led.h"

/*
	��ԭ��ͼ��֪��

	LED0     -> PF9
	LED1     -> PF10
	FSMC_D10 -> PE13
	PSMC_D11 -> PE14

	����Щ��������͵�ƽ���� GPIOx_ODR �Ĵ�����Ӧλ�� 0���� LED ����
	����ߵ�ƽ���� GPIOx_ODR �Ĵ�����Ӧλ�� 1���� LED ��

 */


#ifdef LED_USE_LIBRARY    // ʹ�ÿ⺯����ʽ

/**
 * ���ܣ�  ��ʼ�� LED(ʹ�ÿ⺯����ʽ)
 * ������  ��
 * ����ֵ����
 **/
void led_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	
	// ʹ�� GPIOE �� GPIOF ��ʱ��
	// void RCC_AHB1PeriphClockCmd(uint32_t RCC_AHB1Periph, FunctionalState NewState);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE 
		| RCC_AHB1Periph_GPIOF,	ENABLE);
	
	// ��ʼ�� GPIOE �� GPIOF
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;    // ���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;   // ����
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;     // ����
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

#elif defined(LED_USE_STRUCTURE)   // ʹ�ýṹ�巽ʽ

#if 0

// �����Ǵӿ��ļ��и��Ƶ����ݣ��������ṹ�巽ʽ���ʼĴ���
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
 * ���ܣ�  ��ʼ�� LED - ʹ�ýṹ�巽ʽ
 * ������  ��
 * ����ֵ����
 **/
void led_init(void)
{
	// Ҫʹ GPIOE �� GPIOF ��������Ҫʹ�����ǵ�ʱ��
	// �� STM32F4 �������ֲ�� "2 �洢�������߼ܹ�" ֮ "2.3 �洢��ӳ��" �� "�� 2. STM32F4xx �Ĵ����߽��ַ"
	// �ɲ鵽���������������ߣ�Ҳ���Բ鵽����ı߽��ַ
	// ���飬GPIOE �� GPIOF ������ AHB1 ����
	// �� "6.3.12 RCC AHB1 ����ʱ��ʹ�ܼĴ��� (RCC_AHB1ENR)" �пɲ鵽��GPIOE �� GPIOF ��ʱ�ӷֱ��� [4] �� [5] λ
	// ���ƣ�1 ʹ�ܣ�0 ���á���ƫ�Ƶ�ַΪ 0x30��
	// ÿ���Ĵ�������һ����ַ���� "�� 2. STM32F4xx �Ĵ����߽��ַ" ���Բ鵽 RCC ����ʼ��ַΪ 0x4002 3800 
	// ��ˣ��õ� RCC_AHB1ENR �ĵ�ַΪ 0x40023800 + 0x30
	// 0x3 << 4 �� (1 << 4) | (1 << 5) ���յõ���ֵ���� 0b110000
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= 0x3 << 4;   // [4] �� [5] λ�� 1
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= (1 << 4) | (1 << 5);
	RCC->AHB1ENR |= 0x3 << 4;
	
	// ���� PF9, PF10
	// ���� GPIOF ����ʼ��ַΪ 0x4002 1400 
	// 7.4.1 GPIO �˿�ģʽ�Ĵ��� (GPIOx_MODER) (x = A..I) ƫ�Ƶ�ַ 0x00
	// ģʽ�У�00�����루��λ״̬����01��ͨ�����ģʽ��10�����ù���ģʽ��11��ģ��ģʽ
	// LED �� "ͨ�����ģʽ"���� 01
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	//*((volatile unsigned int *) (0x40021400 + 0x00)) &= ~(0xF << 18);   // 4 ��λ�� 0
	//*((volatile unsigned int *) (0x40021400 + 0x00)) |= 0x5 << 18;      // 4 ��λ��ֵΪ 0b0101
	GPIOF->MODER &= ~(0xF << 18);
	GPIOF->MODER |= 0x5 << 18;
	
	// 7.4.2 GPIO �˿�������ͼĴ��� (GPIOx_OTYPER) (x = A..I) ƫ�Ƶ�ַ 0x04
	// ��������У�0��������죨��λ״̬����1�������©
	// LED ������������źţ����ѡ��������죬�� 0
	// PF9 �� PF10 �ֱ�ռ [9] λ�� [10] λ
	//*((volatile unsigned int *) (0x40021400 + 0x04)) &= ~(0x3 << 9);    // 2 ��λ�� 0
	GPIOF->OTYPER &= ~(0x3 << 9);

	// 7.4.3 GPIO �˿�����ٶȼĴ��� (GPIOx_OSPEEDR) (x = A..I/) ƫ�Ƶ�ַ 0x08
	// ����ٶ��У�00�� 2 MHz�����٣���01�� 25 MHz�����٣���10�� 50 MHz�����٣�
	//             11�� 30 pF ʱΪ 100 MHz�����٣���15 pF ʱΪ 80 MHz ���������ٶȣ���
	// LED ����ٶȿ���ѡ������ѡ����٣��� 11
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	//*((volatile unsigned int *) (0x40021400 + 0x08)) |= 0xF << 18;      // 4 ��λ�� 1
	GPIOF->OSPEEDR |= 0xF << 18;
	
	// 7.4.4 GPIO �˿�����/�����Ĵ��� (GPIOx_PUPDR) (x = A..I/) ƫ�Ƶ�ַ 0x0C
	// ����/������ 4 ��ѡ��00����������������01��������10��������11������
	// LED ����ѡ���������� 01
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	//*((volatile unsigned int *) (0x40021400 + 0x0C)) &= ~(0xF << 18);   // 4 ��λ�� 0
	//*((volatile unsigned int *) (0x40021400 + 0x0C)) |= 0x5 << 18;      // 4 ��λ��ֵΪ 0b0101
	GPIOF->PUPDR &= ~(0xF << 18);
	GPIOF->PUPDR |= 0x5 << 18;
	
	// ���� PE13, PE14
	// ���� GPIOE ����ʼ��ַΪ 0x4002 1000
	// ģʽ�Ĵ��� (GPIOx_MODER)
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	//*((volatile unsigned int *) (0x40021000 + 0x00)) &= ~(0xF << 26);   // 4 ��λ�� 0
	//*((volatile unsigned int *) (0x40021000 + 0x00)) |= 0x5 << 26;      // 4 ��λ��ֵΪ 0b0101
	GPIOE->MODER &= ~(0xF << 26);
	GPIOE->MODER |= 0x5 << 26;
	
	// ������ͼĴ��� (GPIOx_OTYPER)
	// PE13 �� PE14 �ֱ�ռ [13] λ�� [14] λ
	//*((volatile unsigned int *) (0x40021000 + 0x04)) &= ~(0x3 << 13);    // 2 ��λ�� 0
	GPIOE->OTYPER &= ~(0x3 << 13);
	
	// ����ٶȼĴ��� (GPIOx_OSPEEDR)
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	//*((volatile unsigned int *) (0x40021000 + 0x08)) |= 0xF << 26;      // 4 ��λ�� 1
	GPIOE->OSPEEDR |= 0xF << 26;
	
	// ����/�����Ĵ��� (GPIOx_PUPDR) 
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	//*((volatile unsigned int *) (0x40021000 + 0x0C)) &= ~(0xF << 26);   // 4 ��λ�� 0
	//*((volatile unsigned int *) (0x40021000 + 0x0C)) |= 0x5 << 26;      // 4 ��λ��ֵΪ 0b0101
	GPIOE->PUPDR &= ~(0xF << 26);
	GPIOE->PUPDR |= 0x5 << 26;
	
	
	LED0_OFF();
	LED1_OFF();
	LED2_OFF();
	LED3_OFF();
		
}

#elif defined(LED_USE_ORIGINAL)     // ʹ��ԭ����ʽ    

/**
 * ���ܣ�  ��ʼ�� LED - ԭ����ʽ
 * ������  ��
 * ����ֵ����
 **/
void led_init(void)
{
	// Ҫʹ GPIOE �� GPIOF ��������Ҫʹ�����ǵ�ʱ��
	// �� STM32F4 �������ֲ�� "2 �洢�������߼ܹ�" ֮ "2.3 �洢��ӳ��" �� "�� 2. STM32F4xx �Ĵ����߽��ַ"
	// �ɲ鵽���������������ߣ�Ҳ���Բ鵽����ı߽��ַ
	// ���飬GPIOE �� GPIOF ������ AHB1 ����
	// �� "6.3.12 RCC AHB1 ����ʱ��ʹ�ܼĴ��� (RCC_AHB1ENR)" �пɲ鵽��GPIOE �� GPIOF ��ʱ�ӷֱ��� [4] �� [5] λ
	// ���ƣ�1 ʹ�ܣ�0 ���á���ƫ�Ƶ�ַΪ 0x30��
	// ÿ���Ĵ�������һ����ַ���� "�� 2. STM32F4xx �Ĵ����߽��ַ" ���Բ鵽 RCC ����ʼ��ַΪ 0x4002 3800 
	// ��ˣ��õ� RCC_AHB1ENR �ĵ�ַΪ 0x40023800 + 0x30
	// 0x3 << 4 �� (1 << 4) | (1 << 5) ���յõ���ֵ���� 0b110000
	//*((volatile unsigned int *) (0x40023800 + 0x30)) |= 0x3 << 4;   // [4] �� [5] λ�� 1
	*((volatile unsigned int *) (0x40023800 + 0x30)) |= (1 << 4) | (1 << 5);

	// ���� PF9, PF10
	// ���� GPIOF ����ʼ��ַΪ 0x4002 1400 
	// 7.4.1 GPIO �˿�ģʽ�Ĵ��� (GPIOx_MODER) (x = A..I) ƫ�Ƶ�ַ 0x00
	// ģʽ�У�00�����루��λ״̬����01��ͨ�����ģʽ��10�����ù���ģʽ��11��ģ��ģʽ
	// LED �� "ͨ�����ģʽ"���� 01
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	*((volatile unsigned int *) (0x40021400 + 0x00)) &= ~(0xF << 18);   // 4 ��λ�� 0
	*((volatile unsigned int *) (0x40021400 + 0x00)) |= 0x5 << 18;      // 4 ��λ��ֵΪ 0b0101

	// 7.4.2 GPIO �˿�������ͼĴ��� (GPIOx_OTYPER) (x = A..I) ƫ�Ƶ�ַ 0x04
	// ��������У�0��������죨��λ״̬����1�������©
	// LED ������������źţ����ѡ��������죬�� 0
	// PF9 �� PF10 �ֱ�ռ [9] λ�� [10] λ
	*((volatile unsigned int *) (0x40021400 + 0x04)) &= ~(0x3 << 9);    // 2 ��λ�� 0

	// 7.4.3 GPIO �˿�����ٶȼĴ��� (GPIOx_OSPEEDR) (x = A..I/) ƫ�Ƶ�ַ 0x08
	// ����ٶ��У�00�� 2 MHz�����٣���01�� 25 MHz�����٣���10�� 50 MHz�����٣�
	//             11�� 30 pF ʱΪ 100 MHz�����٣���15 pF ʱΪ 80 MHz ���������ٶȣ���
	// LED ����ٶȿ���ѡ������ѡ����٣��� 11
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	*((volatile unsigned int *) (0x40021400 + 0x08)) |= 0xF << 18;      // 4 ��λ�� 1
	
	// 7.4.4 GPIO �˿�����/�����Ĵ��� (GPIOx_PUPDR) (x = A..I/) ƫ�Ƶ�ַ 0x0C
	// ����/������ 4 ��ѡ��00����������������01��������10��������11������
	// LED ����ѡ���������� 01
	// PF9 �� PF10 �ֱ�ռ [19:18] λ�� [21:20] λ
	*((volatile unsigned int *) (0x40021400 + 0x0C)) &= ~(0xF << 18);   // 4 ��λ�� 0
	*((volatile unsigned int *) (0x40021400 + 0x0C)) |= 0x5 << 18;      // 4 ��λ��ֵΪ 0b0101
	
	// ���� PE13, PE14
	// ���� GPIOE ����ʼ��ַΪ 0x4002 1000
	// ģʽ�Ĵ��� (GPIOx_MODER)
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	*((volatile unsigned int *) (0x40021000 + 0x00)) &= ~(0xF << 26);   // 4 ��λ�� 0
	*((volatile unsigned int *) (0x40021000 + 0x00)) |= 0x5 << 26;      // 4 ��λ��ֵΪ 0b0101

	// ������ͼĴ��� (GPIOx_OTYPER)
	// PE13 �� PE14 �ֱ�ռ [13] λ�� [14] λ
	*((volatile unsigned int *) (0x40021000 + 0x04)) &= ~(0x3 << 13);    // 2 ��λ�� 0

	// ����ٶȼĴ��� (GPIOx_OSPEEDR)
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	*((volatile unsigned int *) (0x40021000 + 0x08)) |= 0xF << 26;      // 4 ��λ�� 1

	// ����/�����Ĵ��� (GPIOx_PUPDR) 
	// PE13 �� PE14 �ֱ�ռ [27:26] λ�� [29:28] λ
	*((volatile unsigned int *) (0x40021000 + 0x0C)) &= ~(0xF << 26);   // 4 ��λ�� 0
	*((volatile unsigned int *) (0x40021000 + 0x0C)) |= 0x5 << 26;      // 4 ��λ��ֵΪ 0b0101
}

#endif

