
#include "stm32f4xx.h"
#include "spi.h"

/*
	��ԭ����ͼ�ɵã�
	
	MCU �� SPI1 ����������豸��
	1. W25Q128 FLASH
	2. NRF24L01 ����ͨ��ģ��
	
	W25Q128_SI   -> SPI1 _MOSI -> PB5     ���� SPI1 ����
	W25Q128_SO   -> SPI1_MISO -> PB4     ���� SPI1 ����
	W25Q128_CLK  -> SPI1_SCK  -> PB3     ���� SPI1 ����
	W25Q128_CS   -> F_CS      -> PB14    ���� GPIO ����
	
	NRF24L01_CSN -> NRF_CS    -> PG7     ���� GPIO ����
 */

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
void spi1_init(uint16_t baudrate)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	SPI_InitTypeDef SPI_InitStruct;
	
	// ʹ�� GPIOB, PGIOG ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB | RCC_AHB1Periph_GPIOG, ENABLE);
	
	// ʹ�� SPI1 ʱ��
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);
	
	// ���� PB3, PB4, PB5 ����Ϊ SPI1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource3, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource4, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource5, GPIO_AF_SPI1);
	
	// ���� PB3, PB4, PB5
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5;
	GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// ���� PB14
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// ���� PG7
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOG, &GPIO_InitStruct);
	
	// ���� SPI1
	SPI_InitStruct.SPI_BaudRatePrescaler = baudrate;
	// W25Q128 ֧�� ģʽ0/ģʽ3����������Ϊ ģʽ0
	// ģʽ0��CPOL CPHA = 00
	// ģʽ1��CPOL CPHA = 01
	// ģʽ2��CPOL CPHA = 10
	// ģʽ3��CPOL CPHA = 11
	SPI_InitStruct.SPI_CPHA = SPI_CPHA_1Edge;    // ��һ������ 0
	SPI_InitStruct.SPI_CPOL = SPI_CPOL_Low;      // �͵�ƽ���� 0
	SPI_InitStruct.SPI_CRCPolynomial = 7;        // CRC У�����ʽ
	SPI_InitStruct.SPI_DataSize = SPI_DataSize_8b;   // 8 λ����
	SPI_InitStruct.SPI_Direction = SPI_Direction_2Lines_FullDuplex;  // ˫��ȫ˫��
	SPI_InitStruct.SPI_FirstBit = SPI_FirstBit_MSB;   // MSB �ȷ���
	SPI_InitStruct.SPI_Mode = SPI_Mode_Master;   // ��ģʽ
	SPI_InitStruct.SPI_NSS = SPI_NSS_Soft;       // ��������ģʽѡ��
	SPI_Init(SPI1, &SPI_InitStruct);
	
	// ʹ�� SPI1
	SPI_Cmd(SPI1, ENABLE);
	
	// ������������
	W25Q128_DISABLE();
	NRF24L01_DISABLE();
}

/*
	W25Q128 �Ĵ洢��֯�ṹ��
	1 ҳ = 256 Byte
	1 ���� = 4K Byte = 16 ҳ
	1 �� = 64K Byte = 256 ҳ = 16 ����
	1 Ƭ = 256 ��

	�ɰ� ������32K �顢64K �顢��Ƭ Ϊ��λ������

	״̬�Ĵ�����
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

	���� S1 �� S0 ����
 */

/**
 * ���ܣ�  SPI1 �� W25Q128 ��дһ���ֽ�
 * ������  data Ҫ���͵�һ���ֽ�����
 * ����ֵ����ȡ��һ���ֽ�����
 **/
uint8_t spi1_w25q128_read_write_byte(uint8_t data)
{
	// void SPI_I2S_SendData(SPI_TypeDef* SPIx, uint16_t Data);
	// uint16_t SPI_I2S_ReceiveData(SPI_TypeDef* SPIx);
	// �ȴ����ͷǿ�
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE));
	SPI_I2S_SendData(SPI1, data);
	// �ȴ����շǿ�
	while (RESET == SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE));
	return SPI_I2S_ReceiveData(SPI1);//����ǽ��ܾͷ�������
}

/**
 * ���ܣ�  �������̺��豸 ID
 * ������  ��
 * ����ֵ�����������̺��豸 ID�������� ID �ڸ��ֽڣ��豸 ID �ڵ��ֽ�
 **/
uint16_t spi1_w25q128_read_id(void)
{
	uint16_t id = 0;
	
	// Ƭѡ
	W25Q128_ENABLE();
	
	// ���Ͷ� ID ָ��
	spi1_w25q128_read_write_byte(W25Q128_ID);

	// ���� 3 ���ֽڵ� 0 address
	spi1_w25q128_read_write_byte(0x00);
	spi1_w25q128_read_write_byte(0x00);
	spi1_w25q128_read_write_byte(0x00);
	// ���������� ID 
	id = spi1_w25q128_read_write_byte(W25Q128_DUMMY) << 8;
	// �����豸 ID
	id |= spi1_w25q128_read_write_byte(W25Q128_DUMMY) & 0xFF;
	
	// ȡ��Ƭѡ
	W25Q128_DISABLE();
	
	return id;
}

// ע�⣺�κ�ʱ�򶼿��Զ�״̬�Ĵ�����������д��������д״̬�Ĵ���ʱ�����Զ�״̬�Ĵ���

/**
 * ���ܣ�  ��״̬�Ĵ���
 * ������  ��
 * ����ֵ����
 **/
uint8_t spi1_w25q128_read_sr1(void)
{
	uint8_t sr1 = 0;
	
	// Ƭѡ
	W25Q128_ENABLE();
	
	// ���� ��״̬�Ĵ��� 1 ����
	spi1_w25q128_read_write_byte(W25Q128_SR1);
	sr1 = spi1_w25q128_read_write_byte(W25Q128_DUMMY);
	
	// Ƭѡ
	W25Q128_DISABLE();
	
	return sr1;
}

// ע�⣺������ ҳд�������������������Ƭ������д״̬�Ĵ���������/д��ȫ�Ĵ����ȵ�֮ǰдʹ��

/**
 * ���ܣ�  дʹ��
 * ������  ��
 * ����ֵ����
 **/
void spi1_w25q128_write_enable(void)
{
	// Ƭѡ
	W25Q128_ENABLE();
	
	// ���� дʹ�� ����
	spi1_w25q128_read_write_byte(W25Q128_WEL);
	
	// Ƭѡ
	W25Q128_DISABLE();
}

// ע�⣺���ϵ��д��״̬�Ĵ����󡢲���/д��ȫ�Ĵ�����ҳд������������
//       �������Ƭ�����󡢸�λָ��󣬶����Զ�д����

/**
 * ���ܣ�  д����
 * ������  ��
 * ����ֵ����
 **/
void spi1_w25q128_write_disable(void)
{
	// Ƭѡ
	W25Q128_ENABLE();
	
	// ���� д���� ����
	spi1_w25q128_read_write_byte(W25Q128_WD);
	
	// Ƭѡ
	W25Q128_DISABLE();
}

// ��ҵ��

// �����

// ҳд

// �� n ���ֽ�

