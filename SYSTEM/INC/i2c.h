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
 * ���ܣ�  ��ʼ�� I2C1
 * ������  ��
 * ����ֵ����
 **/
void i2c1_init(void);

/**
 * ���ܣ�  ͨ�� I2C1 �� 24C02 д�벻����һҳ����
 * ������  data ����ָ��
 *         size ���ݴ�С
 *         addr 24C02 Ƭ�ڵ�ַ��0 - 255��
 * ����ֵ���ɹ�����д����ֽ�����ʧ�ܷ��� -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr);

/**
 * ���ܣ�  �� 24C02 �� addr ����ȡ��� size ���ֽڵ� buf ��
 * ������  buf  ��Ŷ�ȡ�����ݻ�����
 *         addr 24C02 �ڶ�ȡ�ĳ�ʼ��ַ
 *         size Ҫ��ȡ���ֽ���
 * ����ֵ���ɹ�����ʵ�ʶ�ȡ���ֽ�����ʧ�ܷ��� -1
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
