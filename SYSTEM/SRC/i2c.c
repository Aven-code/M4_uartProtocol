
#include "stm32f4xx.h"
#include "bitbanding.h"
#include "delay.h"
#include "i2c.h"

/*
	查原理图：
	
	MCU 外接了以下器件：
	1. 24C02 EEPROM
	2. MPU6050 三轴陀螺仪和加速度计
	3. WM8978 声卡
	另外还有一个 P3 对外接口，用于外接其他器件。
	
	IIC_SCL -- PB8 -- 复用于 I2C1_SCL
	IIC_SDA -- PB9 -- 复用于 I2C1_SDA
	
	IIC_SCL 和 IIC_SDA 都外接电源（有上拉电阻），I2C 总线的 SCL 和 SDA 使用开漏实现“线与”功能。
	
*/


#if 1    // MCU 外设

/**
 * 功能：  初始化 I2C1
 * 参数：  无
 * 返回值：无
 **/
void i2c1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	I2C_InitTypeDef I2C_InitStruct;
	
	// 使能 GPIOB 时钟
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	// 使能 I2C1 时钟
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	
	// 复用 PB8, PB9 为 I2C1
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_I2C1);
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_I2C1);
	
	// 配置 PB8, PB9
	GPIO_InitStruct.GPIO_Mode  = GPIO_Mode_AF;   // 复用功能
	GPIO_InitStruct.GPIO_OType = GPIO_OType_OD;    // 开漏输出
	GPIO_InitStruct.GPIO_Pin   = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStruct.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	// 配置 I2C1
	I2C_InitStruct.I2C_Ack                 = I2C_Ack_Disable;
	// MCU 支持 7 位和 10 位地址，这里选择 7 位地址
	I2C_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	// MCU 支持 100KHz 和 400KHz 两种 SCL 时钟
	I2C_InitStruct.I2C_ClockSpeed          = 100000;  // 100KHz 的 SCL 时钟
	
	
	// SCL 时钟每个周期中低电平和高电平时间的比例，称为“占空比”
	// MCU 支持两种快速模式占空比：T low : T high = 2:1, T low : T high = 16:9
	I2C_InitStruct.I2C_DutyCycle           = I2C_DutyCycle_16_9;
	// MCU 支持：I2C, SMBus 设备, SMBus 主机 三种模式
	I2C_InitStruct.I2C_Mode                = I2C_Mode_I2C;
	// 自身地址，当在总线上作为从器件时的地址
	I2C_InitStruct.I2C_OwnAddress1         = MCU_I2C_OWN_ADDRESS;
	// void I2C_Init(I2C_TypeDef* I2Cx, I2C_InitTypeDef* I2C_InitStruct); 
	I2C_Init(I2C1, &I2C_InitStruct);
	
	// 使能 I2C1
	// void I2C_Cmd(I2C_TypeDef* I2Cx, FunctionalState NewState);
	I2C_Cmd(I2C1, ENABLE);
}

// 注意：在对 24C02 进行读写操作时，要结合 MCU 的主模式发送/接收时序 和 24C02 的读写时序

// void I2C_SendData(I2C_TypeDef* I2Cx, uint8_t Data);
// uint8_t I2C_ReceiveData(I2C_TypeDef* I2Cx);
// void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
// void I2C_GenerateSTOP(I2C_TypeDef* I2Cx, FunctionalState NewState);
// ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
// void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);

/**
 * 功能：  通过 I2C1 向 24C02 写入不超过一页数据
 * 参数：  data 数据指针
 *         size 数据大小
 *         addr 24C02 片内地址（0 - 255）
 * 返回值：成功返回写入的字节数，失败返回 -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr)
{
	// count 是当前页剩余空间（字节数）
	uint8_t count = M24C02_PAGE_SIZE - addr % M24C02_PAGE_SIZE;
	//addr 当前页的第几个字节
	uint32_t timeout;
	
	if (size > count)
	{
		size = count;
	}
	
	// 1. 等待 I2C 总线空闲
	// FlagStatus I2C_GetFlagStatus(I2C_TypeDef* I2Cx, uint32_t I2C_FLAG);
	timeout = I2C_TIMEOUT;
	while (timeout-- && (SET == I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 2. 发送起始信号
	// void I2C_GenerateSTART(I2C_TypeDef* I2Cx, FunctionalState NewState);
	I2C_GenerateSTART(I2C1, ENABLE);
	// 检查 EV5
	// ErrorStatus I2C_CheckEvent(I2C_TypeDef* I2Cx, uint32_t I2C_EVENT);
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 3. 发送 7 位 24C02 地址
	// void I2C_Send7bitAddress(I2C_TypeDef* I2Cx, uint8_t Address, uint8_t I2C_Direction);
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Transmitter);
	// 检查 EV6
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)));
	if (0 == timeout)
	{
		return -1;
	}
	// 检查 EV8
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 4. 发送片内写地址，这里的地址指的是	字节地址	，因为一共就255个字节
	I2C_SendData(I2C1, addr);
	// 检查 EV8
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
	if (0 == timeout)
	{
		return -1;
	}
	
	// 5. 发送数据
	// 先发前 size - 1 个字节
	for (count = 0; count < size - 1; count++)
	{
		I2C_SendData(I2C1, data[count]);
		// 检查 EV8
		timeout = I2C_TIMEOUT;
		while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING)));
		if (0 == timeout)
		{
			return count;
		}
	}
	
	// 发送最后一个字节
	I2C_SendData(I2C1, data[count]);
	// 检查 EV8_2
	timeout = I2C_TIMEOUT;
	while (timeout-- && (ERROR == I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED)));
	if (0 == timeout)
	{
		return count;
	}
	count++;
	
	// 6. 发送结束信号
	I2C_GenerateSTOP(I2C1, ENABLE);
	// 等待生成停止位
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
 * 功能：  从 24C02 的 addr 处读取最多 size 个字节到 buf 中
 * 参数：  buf  存放读取的数据缓冲区
 *         addr 24C02 内读取的超始地址
 *         size 要读取的字节数
 * 返回值：成功返回实际读取的字节数，失败返回 -1
 **/
int i2c1_24c02_ReadBuffer(uint8_t *buf, uint8_t addr, uint8_t size)
{
	uint32_t timeout = I2C_TIMEOUT;
	int i;
	
	// 0 个不读取
	if (0 == size)
	{
		return 0;
	}
	
	// 避免超过 24C02 芯片内存范围
	if (addr + size > M24C02_SIZE)
	{
		size = M24C02_SIZE - addr;
	}
	
	// 1. 总线忙等待
  while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 2. 起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	// 测试 EV5 事件并清除它
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 3. 总线寻址 24C02 写
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Transmitter);
	// 测试 EV6 事件并清除它
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
  {
    if((timeout--) == 0) return -1;
  } 
	
	// 4. 24C02 片内寻址写
	I2C_SendData(I2C1, addr);
	// 测试 EV8 事件并清除它
	timeout = I2C_TIMEOUT;
  //while(I2C_GetFlagStatus(I2C1, I2C_FLAG_BTF) == RESET)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTING))   // 检查 EV8
  {
    if((timeout--) == 0) return -1;
  }
	
	// 5. 重复起始位
	I2C_GenerateSTART(I2C1, ENABLE);
	// 测试 EV5 事件并清除它
	timeout = I2C_TIMEOUT;
  while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT))
  {
    if((timeout--) == 0) return -1;
  }
	
	// 6. 总线寻址 24C02 读
	I2C_Send7bitAddress(I2C1, M24C02_ADDRESS, I2C_Direction_Receiver); 
	// 等待地址标志置位(此时地址标志还未清除)
	timeout = I2C_TIMEOUT;
	while(I2C_GetFlagStatus(I2C1, I2C_FLAG_ADDR) == RESET)
	{
		if((timeout--) == 0) return -1;
	}   
	
	// 通过读 SR1 和 SR2 寄存器来清除地址标志(SR1 已读过)
	(void) I2C1->SR2;
	
	// 应答
	I2C_AcknowledgeConfig(I2C1, ENABLE); 
	
	// 7. 开始读取前 size - 1 个字节数据
	for (i = 0; i < size - 1; i++)
	{
		// 等待可读
		timeout = I2C_TIMEOUT;
		//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)    // 接收非空
		while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  // 检查 EV7
		{
			if((timeout--) == 0) return i;
		}
		
		// 读取数据
		buf[i] = I2C_ReceiveData(I2C1);
	}
	
	// 禁用应答、读 DR 寄存器、发送停止位、EV7 共用构成 EV7_1
	// 禁用应答
	I2C_AcknowledgeConfig(I2C1, DISABLE); 
	
	// 8. 停止位
	I2C_GenerateSTOP(I2C1, ENABLE);
	
	// 等待可读
	timeout = I2C_TIMEOUT;
	//while(I2C_GetFlagStatus(I2C1, I2C_FLAG_RXNE) == RESET)
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_RECEIVED))  // 检查 EV7
	{
		if((timeout--) == 0) return i;
	}
		
	// 读取最后一个数据
	buf[i] = I2C_ReceiveData(I2C1);
	
	// 等待停止位被清除
	timeout = I2C_TIMEOUT;
	while(I2C1->CR1 & I2C_CR1_STOP)
	{
		if((timeout--) == 0) return -1;
	}  
	
	// 重新使能应答，准备下一次读
	I2C_AcknowledgeConfig(I2C1, ENABLE);  

	return size;
}

#else    // 模拟方式

void i2c1_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//使能 GPIOB 时钟
	
	//GPIOB8,B9 初始化设置
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
	I2C1_SCL = 1;   
	I2C1_SDA = 1;
}

void i2c1_sda_out(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//普通输出模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}

void i2c1_sda_in(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;//普通输入模式
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;//开漏输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//上拉
	GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化
}

void i2c1_start(void)
{
	i2c1_sda_out();    //sda 线输出
	I2C1_SDA = 1;
	I2C1_SCL = 1;
	delay_us(5);
	I2C1_SDA = 0;//START:when CLK is high,DATA change form high to low
	delay_us(5);
	I2C1_SCL = 0;//钳住 I2C 总线，准备发送或接收数据
}

void i2c1_stop(void)
{
	i2c1_sda_out();//sda 线输出
	I2C1_SCL = 0;
	I2C1_SDA = 0;//STOP:when CLK is high DATA change form low to high
	delay_us(5);
	I2C1_SCL = 1; 
	I2C1_SDA = 1;//发送 I2C 总线结束信号
	delay_us(5);
}

//返回值：1，接收应答失败；0，接收应答成功
u8 i2c1_wait_ack(void)
{
	u8 ucErrTime = 0;
	
	i2c1_sda_in();     //SDA 设置为输入
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
	I2C1_SCL = 0;//时钟输出 0
	delay_us(3); 
	
	return 0;
}

//产生 ACK 应答
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

//不产生 ACK 应答
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
	I2C1_SCL = 0;//拉低时钟开始数据传输
	for(t = 0; t < 8; t++)
	{
		I2C1_SDA = (txd & 0x80) >> 7;
		txd <<= 1;
		delay_us(5);  //对 TEA5767 这三个延时都是必须的  delay_us(2);
		I2C1_SCL = 1;
		delay_us(5);  // delay_us(2);
		I2C1_SCL = 0; 
		//delay_us(4);  // delay_us(2);
	}
}

//读 1 个字节，ack=1 时，发送 ACK，ack=0，发送 nACK
u8 i2c1_read_byte(unsigned char ack)
{
	unsigned char i,receive=0;
	
	i2c1_sda_in();//SDA 设置为输入
	for (i = 0; i < 8; i++)
	{
		I2C1_SCL = 0; 
		delay_us(5);     // delay_us(2);
		I2C1_SCL = 1;
		receive <<= 1;
		if (READ_SDA) receive++;
		delay_us(5);     // delay_us(1);
	}
	if (!ack) i2c1_nack();//发送 nACK
	else i2c1_ack(); //发送 ACK
	
	return receive;
}

// 等待部线空闲
void i2c1_wait_idle(void)
{
	while (!I2C1_SCL || !I2C1_SDA);
}

/**
 * 功能：  通过 I2C1 向 24C02 写入不超过一页数据
 * 参数：  data 数据指针
 *         size 数据大小
 *         addr 24C02 片内地址（0 - 255）
 * 返回值：成功返回写入的字节数，失败返回 -1
 **/
int i2c1_24c02_page_write(uint8_t *data, uint32_t size, uint8_t addr)
{
	// count 是当前页剩余空间（字节数）
	uint8_t count = M24C02_PAGE_SIZE - addr % M24C02_PAGE_SIZE;
	uint32_t timeout;
	
	if (size > count)
	{
		size = count;
	}
	
	// 1. 等待 I2C 总线空闲
	i2c1_wait_idle();
	
	// 2. 发送起始信号
	i2c1_start();
	
	// 3. 发送 7 位 24C02 地址（寻址写）
	i2c1_send_byte(M24C02_ADDRESS);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 4. 发送片内写地址
	i2c1_send_byte(addr);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 5. 发送数据
	// 先发前 size 个字节
	for (count = 0; count < size; count++)
	{
		i2c1_send_byte(data[count]);
		if (1 == i2c1_wait_ack())
		{
			return count;
		}
	}
	
	// 6. 发送结束信号
	i2c1_stop();
	
	//delay_us(5);
	
	return size;
}

/**
 * 功能：  从 24C02 的 addr 处读取最多 size 个字节到 buf 中
 * 参数：  buf  存放读取的数据缓冲区
 *         addr 24C02 内读取的超始地址
 *         size 要读取的字节数
 * 返回值：成功返回实际读取的字节数，失败返回 -1
 **/
int i2c1_24c02_ReadBuffer(uint8_t *buf, uint8_t addr, uint8_t size)
{
	uint32_t timeout = I2C_TIMEOUT;
	int i;
	
	// 0 个不读取
	if (0 == size)
	{
		return 0;
	}
	
	// 避免超过 24C02 芯片内存范围
	if (addr + size > M24C02_SIZE)
	{
		size = M24C02_SIZE - addr;
	}
	
	// 1. 总线忙等待
  i2c1_wait_idle();
	
	// 2. 起始位
	i2c1_start();
	
	// 3. 总线寻址 24C02 写
	i2c1_send_byte(M24C02_ADDRESS);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 4. 24C02 片内寻址写
	i2c1_send_byte(addr);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 5. 重复起始位
	i2c1_start();
	
	// 6. 总线寻址 24C02 读
	i2c1_send_byte(M24C02_ADDRESS | 0x1);
	if (1 == i2c1_wait_ack())
	{
		return -1;
	}
	
	// 7. 开始读取前 size - 1 个字节数据
	for (i = 0; i < size - 1; i++)
	{
		// 读取数据
		buf[i] = i2c1_read_byte(1);
	}

	// 读取最后一个数据
	buf[i] = i2c1_read_byte(0);

	// 8. 停止位
	i2c1_stop();
	
	return size;
}

#endif


