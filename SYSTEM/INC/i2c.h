#ifndef _I2C_H
#define _I2C_H

#include "stm32f4xx.h"

#define M24C02_ADDRESS      0xA0
#define MCU_I2C_OWN_ADDRESS 0xB0
#define M24C02_SIZE         256
#define M24C02_PAGE_SIZE    8
#define M24C02_PAGE_COUNT   (M24C02_SIZE / M24C02_PAGE_SIZE)
#define I2C_TIMEOUT         0xFFFFFUL

/**
 * 功能：  初始化 I2C1
 * 参数：  无
 * 返回值：无
 **/
void i2c1_init(void);

/**
 * 功能：  通过 I2C1 向 24C02 写入不超过一页数据
 * 参数：  data 数据指针
 *         size 数据大小
 *         addr 24C02 片内地址（0 - 255）
 * 返回值：成功返回写入的字节数，失败返回 -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr);

/**
 * 功能：  从 24C02 的 addr 处读取最多 size 个字节到 buf 中
 * 参数：  buf  存放读取的数据缓冲区
 *         addr 24C02 内读取的超始地址
 *         size 要读取的字节数
 * 返回值：成功返回实际读取的字节数，失败返回 -1
 **/
int i2c1_24c02_ReadBuffer(uint8_t *buf, uint8_t addr, uint8_t size);

#define I2C1_SCL PBout(8)
#define I2C1_SDA PBout(9)
#define READ_SDA I2C1_SDA

void i2c1_init(void);
void i2c1_sda_out(void);
void i2c1_sda_in(void);
void i2c1_start(void);
void i2c1_stop(void);
u8 i2c1_wait_ack(void);
void i2c1_ack(void);
void i2c1_nack(void);
void i2c1_send_byte(u8 txd);
u8 i2c1_read_byte(unsigned char ack);

#endif
