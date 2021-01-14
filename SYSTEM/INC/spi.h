#ifndef _SPI_H
#define _SPI_H

#include "bitbanding.h"

#define W25Q128_ENABLE()   do { PBout(14) = 0; } while (0)
#define W25Q128_DISABLE()  do { PBout(14) = 1; } while (0)

#define NRF24L01_ENABLE()  do { PGout(7) = 0; } while (0)
#define NRF24L01_DISABLE() do { PGout(7) = 1; } while (0)

// 指令
#define W25Q128_WD      0x04
#define W25Q128_SR1     0x05
#define W25Q128_WEL     0x06
#define W25Q128_ID      0x90

#define W25Q128_DUMMY   0xFF

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
void spi1_init(uint16_t baudrate);

/**
 * 功能：  SPI1 与 W25Q128 读写一个字节
 * 参数：  data 要发送的一个字节数据
 * 返回值：读取的一个字节数据
 **/
uint8_t spi1_w25q128_read_write_byte(uint8_t data);

/**
 * 功能：  读制造商和设备 ID
 * 参数：  无
 * 返回值：返回制造商和设备 ID，制造商 ID 在高字节，设备 ID 在低字节
 **/
uint16_t spi1_w25q128_read_id(void);

/**
 * 功能：  写使能
 * 参数：  无
 * 返回值：无
 **/
void spi1_w25q128_write_enable(void);

/**
 * 功能：  写禁用
 * 参数：  无
 * 返回值：无
 **/
void spi1_w25q128_write_disable(void);

#endif
