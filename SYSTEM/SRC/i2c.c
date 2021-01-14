
#include "stm32f4xx.h"
#include "bitbanding.h"
#include "delay.h"
#include "i2c.h"

/*
	��ԭ��ͼ��
	
	MCU ���������������
	1. 24C02 EEPROM
	2. MPU6050 ���������Ǻͼ��ٶȼ�
	3. WM8978 ����
	���⻹��һ�� P3 ����ӿڣ������������������
	
	IIC_SCL -- PB8 -- ������ I2C1_SCL
	IIC_SDA -- PB9 -- ������ I2C1_SDA
	
	IIC_SCL �� IIC_SDA ����ӵ�Դ�����������裩��I2C ���ߵ� SCL �� SDA ʹ�ÿ�©ʵ�֡����롱���ܡ�
	
*/


#if 1    // MCU ����

/**
 * ���ܣ�  ��ʼ�� I2C1
 * ������  ��
 * ����ֵ����
 **/
void i2c1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	
	// ʹ�� GPIOB ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// ʹ�� I2C1 ʱ��
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// ���� PB8, PB9 Ϊ I2C1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
	// ���� PB8, PB9
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;   // ���ù���
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;    // ��©���
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// ���� I2C1
	I2C_InitStruct.I2C_Ack                 = I2C_Ack_Disable;
	// MCU ֧�� 7 λ�� 10 λ��ַ������ѡ�� 7 λ��ַ
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	// MCU ֧�� 100KHz �� 400KHz ���� SCL ʱ��
	I2C_InitStruct.I2C_ClockSpeed          = 100000;  // 100KHz �� SCL ʱ��
	
	
	// SCL ʱ��ÿ�������е͵�ƽ�͸ߵ�ƽʱ��ı�������Ϊ��ռ�ձȡ�
	// MCU ֧�����ֿ���ģʽռ�ձȣ�T low : T high = 2:1, T low : T high = 16:9
	I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_16_9;
	// MCU ֧�֣�I2C, SMBus �豸, SMBus ���� ����ģʽ
	I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;
	// �����ַ��������������Ϊ������ʱ�ĵ�ַ
	I2C_InitStruct.I2C_OwnAddress1         = MCU_I2C_OWN_ADDRESS;
	// void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct); 
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// ʹ�� I2C1
	// void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
	I2C_Cmd(I2C1, ENABLE);
}

// ע�⣺�ڶ� 24C02 ���ж�д����ʱ��Ҫ��� MCU ����ģʽ����/����ʱ�� �� 24C02 �Ķ�дʱ��

// void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
// uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
// void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
// void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
// ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
// void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);

/**
 * ���ܣ�  ͨ�� I2C1 �� 24C02 д�벻����һҳ����
 * ������  data ����ָ��
 *         size ���ݴ�С
 *         addr 24C02 Ƭ�ڵ�ַ��0 - 255��
 * ����ֵ���ɹ�����д����ֽ�����ʧ�ܷ��� -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr)
{
	// count �ǵ�ǰҳʣ��ռ䣨�ֽ�����
	uint8_t count = M24C02_PAGE_SIZE - addr % M24C02_PAGE_SIZE;
	//addr ��ǰҳ�ĵڼ����ֽ�
	uint32_t timeout;
	
	if (size > count)
	{
		size = count;
	}
	
	// 1. �ȴ� I2C ���߿���
	// FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
	timeout = I2C_TIMEOUT;
	while (timeout-- && (SET == I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 2. ������ʼ�ź�
	// void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
	I2C_GenerateSTART(I2C1, ENABLE);
	// ��� EV5
	// ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 3. ���� 7 λ 24C02 ��ַ
	// void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Transmitter);
	// ��� EV6
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
	if (0 == timeout)
	{
		return -1;
	}
	// ��� EV8
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 4. ����Ƭ��д��ַ������ĵ�ַָ����	�ֽڵ�ַ	����Ϊһ����255���ֽ�
	I2C_SendData(I2C1, addr);
	// ��� EV8
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 5. ��������
	// �ȷ�ǰ size - 1 ���ֽ�
	for (count = 0; count < size - 1; count++)
	{
		I2C_SendData(I2C1, data[count]);
		// ��� EV8
		timeout = I2C_TIMEOUT;
		while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
		if (0 == timeout)
		{
			return count;
		}
	}
	
	// �������һ���ֽ�
	I2C_SendData(I2C1, data[count]);
	// ��� EV8_2
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
	if (0 == timeout)
	{
		return count;
	}
	count++;
	
	// 6. ���ͽ����ź�
	I2C_GenerateSTOP(I2C1, ENABLE);
	// �ȴ�����ֹͣλ
	timeout = I2C_TIMEOUT;
	while (timeout-- && (I2C1->CR1 & I2C_CR1_STOP));
	if (0 == timeout)
	{
		return count;
	}
	
	//delay_us(5);
	
	return size;
}

/**
 * ���ܣ�  �� 24C02 �� addr ����ȡ��� size ���ֽڵ� buf ��
 * ������  buf  ��Ŷ�ȡ�����ݻ�����
 *         addr 24C02 �ڶ�ȡ�ĳ�ʼ��ַ
 *         size Ҫ��ȡ���ֽ���
 * ����ֵ���ɹ�����ʵ�ʶ�ȡ���ֽ�����ʧ�ܷ��� -1
 **/
int i2c1_24c02_ReadBuffer(uint8_t *buf, uint8_t addr, uint8_t size)
{
	uint32_t timeout = I2C_TIMEOUT;
	int i;
	
	// 0 ������ȡ
	if (0 == size)
	{
		return 0;
	}
	
	// ���ⳬ�� 24C02 оƬ�ڴ淶Χ
	if (addr + size > M24C02_SIZE)
	{
		size = M24C02_SIZE - addr;
	}
	
	// 1. ����æ�ȴ�
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 2. ��ʼλ
	I2C_GenerateSTART(I2C1, ENABLE);
	// ���� EV5 �¼��������
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 3. ����Ѱַ 24C02 д
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Transmitter);
	// ���� EV6 �¼��������
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((timeout--) == 0) return -1;
  } 
	
	// 4. 24C02 Ƭ��Ѱַд
	I2C_SendData(I2C1, addr);
	// ���� EV8 �¼��������
	timeout = I2C_TIMEOUT;
  //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))   // ��� EV8
  {
    if((timeout--) == 0) return -1;
  }
	
	// 5. �ظ���ʼλ
	I2C_GenerateSTART(I2C1, ENABLE);
	// ���� EV5 �¼��������
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 6. ����Ѱַ 24C02 ��
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Receiver); 
	// �ȴ���ַ��־��λ(��ʱ��ַ��־��δ���)
	timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
	{
		if((timeout--) == 0) return -1;
	}   
	
	// ͨ���� SR1 �� SR2 �Ĵ����������ַ��־(SR1 �Ѷ���)
	(void) I2C1->SR2;
	
	// Ӧ��
	I2C_AcknowledgeConfig(I2C1, ENABLE); 
	
	// 7. ��ʼ��ȡǰ size - 1 ���ֽ�����
	for (i = 0; i < size - 1; i++)
	{
		// �ȴ��ɶ�
		timeout = I2C_TIMEOUT;
		//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)    // ���շǿ�
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  // ��� EV7
		{
			if((timeout--) == 0) return i;
		}
		
		// ��ȡ����
		buf[i] = I2C_ReceiveData(I2C1);
	}
	
	// ����Ӧ�𡢶� DR �Ĵ���������ֹͣλ��EV7 ���ù��� EV7_1
	// ����Ӧ��
	I2C_AcknowledgeConfig(I2C1, DISABLE); 
	
	// 8. ֹͣλ
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// �ȴ��ɶ�
	timeout = I2C_TIMEOUT;
	//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  // ��� EV7
	{
		if((timeout--) == 0) return i;
	}
		
	// ��ȡ���һ������
	buf[i] = I2C_ReceiveData(I2C1);
	
	// �ȴ�ֹͣλ�����
	timeout = I2C_TIMEOUT;
	while(I2C1->CR1 & I2C_CR1_STOP)
	{
		if((timeout--) == 0) return -1;
	}  
	
	// ����ʹ��Ӧ��׼����һ�ζ�
	I2C_AcknowledgeConfig(I2C1, ENABLE);  

	return size;
}

#else    // ģ�ⷽʽ

void i2c1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ�� GPIOB ʱ��
	
	//GPIOB8,B9 ��ʼ������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	I2C1_SCL = 1;   
	I2C1_SDA = 1;
}

void i2c1_sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}

void i2c1_sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//��ͨ����ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//��©���
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
}

void i2c1_start(void)
{
	i2c1_sda_out();    //sda �����
	I2C1_SDA = 1;
	I2C1_SCL = 1;
	delay_us(5);
	I2C1_SDA = 0;//START:when CLK is high,DATA change form high to low
	delay_us(5);
	I2C1_SCL = 0;//ǯס I2C ���ߣ�׼�����ͻ��������
}

void i2c1_stop(void)
{
	i2c1_sda_out();//sda �����
	I2C1_SCL = 0;
	I2C1_SDA = 0;//STOP:when CLK is high DATA change form low to high
	delay_us(5);
	I2C1_SCL = 1; 
	I2C1_SDA = 1;//���� I2C ���߽����ź�
	delay_us(5);
}

//����ֵ��1������Ӧ��ʧ�ܣ�0������Ӧ��ɹ�
u8 i2c1_wait_ack(void)
{
	u8 ucErrTime = 0;
	
	i2c1_sda_in();     //SDA ����Ϊ����
	I2C1_SDA = 1;
	delay_us(5);       // delay_us(1);
	I2C1_SCL = 1;
	delay_us(2);       // delay_us(1);
	while (READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime > 250)
		{
			i2c1_stop();
			return 1;
		}
	}
	I2C1_SCL = 0;//ʱ����� 0
	delay_us(3); 
	
	return 0;
}

//���� ACK Ӧ��
void i2c1_ack(void)
{
	I2C1_SCL = 0;
	i2c1_sda_out();
	I2C1_SDA = 0;
	delay_us(5);      // delay_us(2);
	I2C1_SCL = 1;
	delay_us(5);      // delay_us(2);
	I2C1_SCL = 0;
}

//������ ACK Ӧ��
void i2c1_nack(void)
{
	I2C1_SCL = 0;
	i2c1_sda_out();
	I2C1_SDA = 1;
	delay_us(5);      // delay_us(2);
	I2C1_SCL = 1;
	delay_us(5);      // delay_us(2);
	I2C1_SCL = 0;
}

void i2c1_send_byte(u8 txd)
{
	u8 t;
	
	i2c1_sda_out();
	I2C1_SCL = 0;//����ʱ�ӿ�ʼ���ݴ���
	for(t = 0; t < 8; t++)
	{
		I2C1_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(5);  //�� TEA5767 ��������ʱ���Ǳ����  delay_us(2);
		I2C1_SCL = 1;
		delay_us(5);  // delay_us(2);
		I2C1_SCL = 0; 
		//delay_us(4);  // delay_us(2);
	}
}

//�� 1 ���ֽڣ�ack=1 ʱ������ ACK��ack=0������ nACK
u8 i2c1_read_byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	i2c1_sda_in();//SDA ����Ϊ����
	for (i = 0; i < 8; i++)
	{
		I2C1_SCL = 0; 
		delay_us(5);     // delay_us(2);
		I2C1_SCL = 1;
		receive <<= 1;
		if (READ_SDA) receive++;
		delay_us(5);     // delay_us(1);
	}
	if (!ack) i2c1_nack();//���� nACK
	else i2c1_ack(); //���� ACK
	
	return receive;
}

// �ȴ����߿���
void i2c1_wait_idle(void)
{
	while (!I2C1_SCL || !I2C1_SDA);
}

/**
 * ���ܣ�  ͨ�� I2C1 �� 24C02 д�벻����һҳ����
 * ������  data ����ָ��
 *         size ���ݴ�С
 *         addr 24C02 Ƭ�ڵ�ַ��0 - 255��
 * ����ֵ���ɹ�����д����ֽ�����ʧ�ܷ��� -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr)
{
	// count �ǵ�ǰҳʣ��ռ䣨�ֽ�����
	uint8_t count = M24C02_PAGE_SIZE - addr % M24C02_PAGE_SIZE;
	uint32_t timeout;
	
	if (size > count)
	{
		size = count;
	}
	
	// 1. �ȴ� I2C ���߿���
	i2c1_wait_idle();
	
	// 2. ������ʼ�ź�
	i2c1_start();
	
	// 3. ���� 7 λ 24C02 ��ַ��Ѱַд��
	i2c1_send_byte(M24C02_ADDRESS);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 4. ����Ƭ��д��ַ
	i2c1_send_byte(addr);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 5. ��������
	// �ȷ�ǰ size ���ֽ�
	for (count = 0; count < size; count++)
	{
		i2c1_send_byte(data[count]);
		if (1 == i2c1_wait_ack())
		{
			return count;
		}
	}
	
	// 6. ���ͽ����ź�
	i2c1_stop();
	
	//delay_us(5);
	
	return size;
}

/**
 * ���ܣ�  �� 24C02 �� addr ����ȡ��� size ���ֽڵ� buf ��
 * ������  buf  ��Ŷ�ȡ�����ݻ�����
 *         addr 24C02 �ڶ�ȡ�ĳ�ʼ��ַ
 *         size Ҫ��ȡ���ֽ���
 * ����ֵ���ɹ�����ʵ�ʶ�ȡ���ֽ�����ʧ�ܷ��� -1
 **/
int i2c1_24c02_ReadBuffer(uint8_t *buf, uint8_t addr, uint8_t size)
{
	uint32_t timeout = I2C_TIMEOUT;
	int i;
	
	// 0 ������ȡ
	if (0 == size)
	{
		return 0;
	}
	
	// ���ⳬ�� 24C02 оƬ�ڴ淶Χ
	if (addr + size > M24C02_SIZE)
	{
		size = M24C02_SIZE - addr;
	}
	
	// 1. ����æ�ȴ�
  i2c1_wait_idle();
	
	// 2. ��ʼλ
	i2c1_start();
	
	// 3. ����Ѱַ 24C02 д
	i2c1_send_byte(M24C02_ADDRESS);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 4. 24C02 Ƭ��Ѱַд
	i2c1_send_byte(addr);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 5. �ظ���ʼλ
	i2c1_start();
	
	// 6. ����Ѱַ 24C02 ��
	i2c1_send_byte(M24C02_ADDRESS | 0x1);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 7. ��ʼ��ȡǰ size - 1 ���ֽ�����
	for (i = 0; i < size - 1; i++)
	{
		// ��ȡ����
		buf[i] = i2c1_read_byte(1);
	}

	// ��ȡ���һ������
	buf[i] = i2c1_read_byte(0);

	// 8. ֹͣλ
	i2c1_stop();
	
	return size;
}

#endif


