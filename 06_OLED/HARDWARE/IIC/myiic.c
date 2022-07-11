#include "myiic.h"
#include "delay.h"
#include "stm32f10x_i2c.h"

/**********************************************************
	Initialize GPIO
	Input:
		-I2C_channel(u8): I2C Channel Select(1 or 2)
	Output: void
**********************************************************/
static void I2C_GPIO_Config(u8 I2C_channel)
{
	GPIO_InitTypeDef GPIO_InitStructure;
#if USE_LIB

	/* 使能与 I2C 有关的时钟 */
	if(I2C_channel==1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	else if(I2C_channel==2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* I2C_SCL、I2C_SDA*/
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // 开漏输出
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//使能GPIOB时钟
	
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	if(I2C_channel==1)
		GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	  //PB6,PB7 输出高
	else if(I2C_channel==2)
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB10,PB11 输出高
#endif
}

//初始化IIC
void IIC_Init(u8 I2C_channel)
{
	I2C_GPIO_Config(I2C_channel);
#if USE_LIB
	I2C_Mode_Config(I2C_channel);	
#endif
}

//等待应答信号到来
//返回值：1，接收应答失败
//        0，接收应答成功
u8 IIC_Wait_Ack(u8 I2C_channel, uint32_t I2C_EVENT)
{
#if USE_LIB
	I2C_TypeDef* I2Cx;
	__IO uint32_t I2C_time = I2CT_FLAG_TIMEOUT;
	/* 检测 EV6 事件并清除标志*/
	if(I2C_channel == 1)
		I2Cx = I2C1;
	else if(I2C_channel == 2)
		I2Cx = I2C2;
	else
		I2Cx = I2C1;
	
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT))
	{
		if((I2C_time--) == 0) return I2C_TIMEOUT_UserCallback(I2C_EVENT);
	}
#else
	u8 ucErrTime=0;
	SDA_IN();      //SDA设置为输入  
	IIC_SDA=1;delay_us(1);	   
	IIC_SCL=1;delay_us(1);	 
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	IIC_SCL=0;//时钟输出0 	   
#endif 
	return 0;
}


#if USE_LIB

static void I2C_Mode_Config(u8 I2C_channel)
{
	I2C_InitTypeDef I2C_InitStructure;
 
	/* I2C 配置 */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
 
	/* 高电平数据稳定，低电平数据变化 SCL 时钟线的占空比 */
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
 
	I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
 
	/* I2C 的寻址模式 */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
 
	/* 通信速率 */
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
 
	if(I2C_channel==1)
	{
		/* I2C 初始化 */
		I2C_Init(I2C1, &I2C_InitStructure);
	 
		/* 使能 I2C */
		I2C_Cmd(I2C1, ENABLE);
	}
	else if(I2C_channel==2)
	{
		/* I2C 初始化 */
		I2C_Init(I2C2, &I2C_InitStructure);
	 
		/* 使能 I2C */
		I2C_Cmd(I2C2, ENABLE);
	}
}

/**
* @brief I2C 等待事件超时的情况下会调用这个函数来处理
* @param errorCode：错误代码，可以用来定位是哪个环节出错.
* @retval 返回 0，表示 IIC 读取失败.
*/
static uint32_t I2C_TIMEOUT_UserCallback(uint32_t errorCode)
{
//	/* 使用串口 printf 输出错误信息，方便调试 */
//	EEPROM_ERROR("I2C 等待超时!errorCode = %d",errorCode);
	return 0;
}

#else

//产生IIC起始信号
void IIC_Start(void)
{
	SDA_OUT();     //sda线输出
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//钳住I2C总线，准备发送或接收数据 
}

//产生IIC停止信号
void IIC_Stop(void)
{
	SDA_OUT();//sda线输出
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//发送I2C总线结束信号
	delay_us(4);		
}

//产生ACK应答
void IIC_Ack(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=0;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//不产生ACK应答		    
void IIC_NAck(void)
{
	IIC_SCL=0;
	SDA_OUT();
	IIC_SDA=1;
	delay_us(2);
	IIC_SCL=1;
	delay_us(2);
	IIC_SCL=0;
}

//IIC发送一个字节
//返回从机有无应答
//1，有应答
//0，无应答			  
void IIC_Send_Byte(u8 txd)
{
  u8 t;   
	SDA_OUT(); 	    
  IIC_SCL=0;//拉低时钟开始数据传输
  for(t=0;t<8;t++)
  {            
    IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	  
		delay_us(2);   //对TEA5767这三个延时都是必须的
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
  }
}

//读1个字节，ack=1时，发送ACK，ack=0，发送nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA设置为输入
  for(i=0;i<8;i++ )
	{
    IIC_SCL=0; 
    delay_us(2);
		IIC_SCL=1;
    receive<<=1;
    if(READ_SDA)
			receive++;   
		delay_us(1); 
   }					 
   if (!ack)
		IIC_NAck();//发送nACK
   else
		IIC_Ack(); //发送ACK   
   return receive;
}
#endif 

/*
* Send a byte to I2C hardware
*/
u8 I2C_ByteWrite(u8 I2C_channel, u8 addr, u8 pBuffer, u8 WriteAddr)
{
#if USE_LIB
	I2C_TypeDef* I2Cx;
	if(I2C_channel==1)
		I2Cx = I2C1;
	else if(I2C_channel==2)
		I2Cx = I2C2;
	else
		I2Cx = I2C1;
	
	/* 产生 I2C 起始信号 */
	I2C_GenerateSTART(I2Cx, ENABLE);
	/* 检测 EV5 事件并清除标志*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_MODE_SELECT);

	/* 发送 EEPROM 设备地址 */
	I2C_Send7bitAddress(I2Cx, addr, I2C_Direction_Transmitter);
	/* 检测 EV6 事件并清除标志*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
 
	/* 发送要写入的 EEPROM 内部地址(即 EEPROM 内部存储器的地址) */
	I2C_SendData(I2Cx, WriteAddr);
	/* 检测 EV8 事件并清除标志*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	/* 发送一字节要写入的数据 */
	I2C_SendData(I2Cx, pBuffer);
	/* 检测 EV8 事件并清除标志*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
 
	/* 发送停止信号 */
	I2C_GenerateSTOP(I2Cx, ENABLE); 
#else
		IIC_Start(); //起始信号
		IIC_Send_Byte(addr);//写器件地址
		IIC_Wait_Ack(2, 0);
		IIC_Send_Byte(WriteAddr);//内部寄存器地址
		IIC_Wait_Ack(2, 0);
		IIC_Send_Byte(pBuffer);//内部寄存器数据
		IIC_Wait_Ack(2, 0);
		IIC_Stop();
#endif 
	return 1;
}
