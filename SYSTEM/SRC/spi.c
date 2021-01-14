
#include "stm32f4xx.h"
#include "spi.h"

/*
	查原理轮图可得：
	
	MCU 的 SPI1 外接了两个设备：
	1. W25Q128 FLASH
	2. NRF24L01 无线通信模块
	
	W25Q128_SI   -> SPI1 _MOSI -> PB5     （由 SPI1 管理）
	W25Q128_SO   -> SPI1_MISO -> PB4     （由 SPI1 管理）
	W25Q128_CLK  -> SPI1_SCK  -> PB3     （由 SPI1 管理）
	W25Q128_CS   -> F_CS      -> PB14    （由 GPIO 管理）
	
	NRF24L01_CSN -> NRF_CS    -> PG7     （由 GPIO 管理）
 */

/**
 * 功能：  初始化 SPI1
 * 参数：  baudrate 波特率。根据所在总线频率进行分频，可取值如下：
 *              SPI_BaudRatePrescaler_2
 *              SPI_BaudRatePrescaler_4
 *              SPI_BaudRatePrescaler_8
 *              SPI_BaudRatePrescaler_16
 *              SPI_BaudRatePrescaler_32
 *              SPI_BaudRatePrescaler_64
 *              SPI_BaudRatePrescaler_128
 *              SPI_BaudRatePrescaler_256
 * 返回值：无
 **/
void spi1_init(uint16_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// 使能 GPIOB, PGIOG 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOG, ENABLE);
	
	// 使能 SPI1 时钟
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	// 配置 PB3, PB4, PB5 复用为 SPI1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	// 配置 PB3, PB4, PB5
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 配置 PB14
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 配置 PG7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	// 配置 SPI1
	SPI_InitStruct.SPI_BaudRatePrescaler = baudrate;
	// W25Q128 支持 模式0/模式3，这里配置为 模式0
	// 模式0：CPOL CPHA = 00
	// 模式1：CPOL CPHA = 01
	// 模式2：CPOL CPHA = 10
	// 模式3：CPOL CPHA = 11
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;    // 第一个边沿 0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;      // 低电平空闲 0
	SPI_InitStruct.SPI_CRCPolynomial = 7;        // CRC 校验多项式
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;   // 8 位数据
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // 双线全双工
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;   // MSB 先发送
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;   // 主模式
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;       // 软件管理从模式选择
	SPI_Init(SPI1, &SPI_InitStruct);
	
	// 使能 SPI1
	SPI_Cmd(SPI1, ENABLE);
	
	// 禁用所有外设
	W25Q128_DISABLE();
	NRF24L01_DISABLE();
}

/*
	W25Q128 的存储组织结构：
	1 页 = 256 Byte
	1 扇区 = 4K Byte = 16 页
	1 块 = 64K Byte = 256 页 = 16 扇区
	1 片 = 256 块

	可按 扇区、32K 块、64K 块、整片 为单位擦除。

	状态寄存器：
                                     S7  S6  S5 S4  S3  S2  S1  S0
                                    SRP0 SEC TB BP2 BP1 BP0 WEL BUSY
                                     |    |  |  |   |   |    |   |
Status Register Protect 0------------/    |  |  |   |   |    |   |
(Volatile/Non-Volatile Writable)          |  |  |   |   |    |   |  
Sector Protect Bit       -----------------/  |  |   |   |    |   | 
(Volatile/Non-Volatile Writable)             |  |   |   |    |   | 
Top/Bottom Protect Bit   --------------------/  |   |   |    |   |
(Volatile/Non-Volatile Writable)                |   |   |    |   |
Block Protect Bits       -----------------------/---/---/    |   |
(Volatile/Non-Volatile Writable)                             |   |
Write Enable Latch       ------------------------------------/   |
(Status-Only)                                                    |
Erase/Write In Progress  ----------------------------------------/
(Status-Only)

	其中 S1 和 S0 常用
 */

/**
 * 功能：  SPI1 与 W25Q128 读写一个字节
 * 参数：  data 要发送的一个字节数据
 * 返回值：读取的一个字节数据
 **/
uint8_t spi1_w25q128_read_write_byte(uint8_t data)
{
	// void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
	// uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
	// 等待发送非空
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, data);
	// 等待接收非空
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	return SPI_I2S_ReceiveData(SPI1);//如果是接受就返回数据
}

/**
 * 功能：  读制造商和设备 ID
 * 参数：  无
 * 返回值：返回制造商和设备 ID，制造商 ID 在高字节，设备 ID 在低字节
 **/
uint16_t spi1_w25q128_read_id(void)
{
	uint16_t id = 0;
	
	// 片选
	W25Q128_ENABLE();
	
	// 发送读 ID 指令
	spi1_w25q128_read_write_byte(W25Q128_ID);

	// 发送 3 个字节的 0 address
	spi1_w25q128_read_write_byte(0x00);
	spi1_w25q128_read_write_byte(0x00);
	spi1_w25q128_read_write_byte(0x00);
	// 接收制造商 ID 
	id = spi1_w25q128_read_write_byte(W25Q128_DUMMY) << 8;
	// 接收设备 ID
	id |= spi1_w25q128_read_write_byte(W25Q128_DUMMY) & 0xFF;
	
	// 取消片选
	W25Q128_DISABLE();
	
	return id;
}

// 注意：任何时候都可以读状态寄存器，甚至在写、擦除、写状态寄存器时都可以读状态寄存器

/**
 * 功能：  读状态寄存器
 * 参数：  无
 * 返回值：无
 **/
uint8_t spi1_w25q128_read_sr1(void)
{
	uint8_t sr1 = 0;
	
	// 片选
	W25Q128_ENABLE();
	
	// 发送 读状态寄存器 1 命令
	spi1_w25q128_read_write_byte(W25Q128_SR1);
	sr1 = spi1_w25q128_read_write_byte(W25Q128_DUMMY);
	
	// 片选
	W25Q128_DISABLE();
	
	return sr1;
}

// 注意：必须在 页写、扇区擦除、块擦除、片擦除、写状态寄存器、擦除/写安全寄存器等等之前写使能

/**
 * 功能：  写使能
 * 参数：  无
 * 返回值：无
 **/
void spi1_w25q128_write_enable(void)
{
	// 片选
	W25Q128_ENABLE();
	
	// 发送 写使能 命令
	spi1_w25q128_read_write_byte(W25Q128_WEL);
	
	// 片选
	W25Q128_DISABLE();
}

// 注意：在上电后、写完状态寄存器后、擦除/写安全寄存器后、页写后、扇区擦除后、
//       块擦除后、片擦除后、复位指令后，都会自动写禁用

/**
 * 功能：  写禁用
 * 参数：  无
 * 返回值：无
 **/
void spi1_w25q128_write_disable(void)
{
	// 片选
	W25Q128_ENABLE();
	
	// 发送 写禁用 命令
	spi1_w25q128_read_write_byte(W25Q128_WD);
	
	// 片选
	W25Q128_DISABLE();
}

// 作业：

// 块擦除

// 页写

// 读 n 个字节

