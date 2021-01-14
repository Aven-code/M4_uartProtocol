#ifndef _SPI_H
#define _SPI_H

#include "bitbanding.h"

#define W25Q128_ENABLE()   do { PBout(14) = 0; } while (0)
#define W25Q128_DISABLE()  do { PBout(14) = 1; } while (0)

#define NRF24L01_ENABLE()  do { PGout(7) = 0; } while (0)
#define NRF24L01_DISABLE() do { PGout(7) = 1; } while (0)

// ָ��
#define W25Q128_WD      0x04
#define W25Q128_SR1     0x05
#define W25Q128_WEL     0x06
#define W25Q128_ID      0x90

#define W25Q128_DUMMY   0xFF

/**
 * ���ܣ�  ��ʼ�� SPI1
 * ������  baudrate �����ʡ�������������Ƶ�ʽ��з�Ƶ����ȡֵ���£�
 *              SPI_BaudRatePrescaler_2
 *              SPI_BaudRatePrescaler_4
 *              SPI_BaudRatePrescaler_8
 *              SPI_BaudRatePrescaler_16
 *              SPI_BaudRatePrescaler_32
 *              SPI_BaudRatePrescaler_64
 *              SPI_BaudRatePrescaler_128
 *              SPI_BaudRatePrescaler_256
 * ����ֵ����
 **/
void spi1_init(uint16_t baudrate);

/**
 * ���ܣ�  SPI1 �� W25Q128 ��дһ���ֽ�
 * ������  data Ҫ���͵�һ���ֽ�����
 * ����ֵ����ȡ��һ���ֽ�����
 **/
uint8_t spi1_w25q128_read_write_byte(uint8_t data);

/**
 * ���ܣ�  �������̺��豸 ID
 * ������  ��
 * ����ֵ�����������̺��豸 ID�������� ID �ڸ��ֽڣ��豸 ID �ڵ��ֽ�
 **/
uint16_t spi1_w25q128_read_id(void);

/**
 * ���ܣ�  дʹ��
 * ������  ��
 * ����ֵ����
 **/
void spi1_w25q128_write_enable(void);

/**
 * ���ܣ�  д����
 * ������  ��
 * ����ֵ����
 **/
void spi1_w25q128_write_disable(void);

#endif
