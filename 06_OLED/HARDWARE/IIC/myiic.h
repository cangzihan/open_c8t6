#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

#define USE_LIB 1

#if USE_LIB
 
/* STM32 I2C Fast Mode */
#define I2C_Speed 200000 //* Disable some hardware, this speed could be set to 400k

/* 这个地址只要与 STM32 外挂的 I2C 器件地址不一样即可 */
#define I2Cx_OWN_ADDRESS7 0X0A

/*等待超时时间*/
#define I2CT_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2CT_LONG_TIMEOUT         ((uint32_t)(10 * I2CT_FLAG_TIMEOUT))

static void I2C_Mode_Config(u8 I2C_channel);

#else
//////////////////////////////////////////////////////////////////////////////////	 
//PB7
// CRL 1000 1111 1111 1111  1111 1111 1111 1111
// CRL 0011 1111 1111 1111  1111 1111 1111 1111
//Sooooooo PB 11
// CRH 1111 1111 1111 1111  1000 1111 1111 1111
// CRH 1111 1111 1111 1111  0011 1111 1111 1111
//////////////////////////////////////////////////////////////////////////////////

//IO方向设置
 
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}

//IO操作函数	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //输入SDA

void IIC_Start(void);				//发送IIC开始信号
void IIC_Stop(void);	  		//发送IIC停止信号
void IIC_Send_Byte(u8 txd);	//IIC发送一个字节
void IIC_Ack(void);					//IIC发送ACK信号
void IIC_NAck(void);				//IIC不发送ACK信号 
u8 IIC_Read_Byte(unsigned char ack);//IIC读取一个字节

#endif

//IIC所有操作函数
void IIC_Init(u8 I2C_channel);           //初始化IIC的IO口				
u8 IIC_Wait_Ack(u8 I2C_channel, uint32_t I2C_EVENT); 	  //IIC等待ACK信号

static uint32_t I2C_TIMEOUT_UserCallback(uint32_t errorCode);
static void I2C_GPIO_Config(u8 I2C_channel);
u8 I2C_ByteWrite(u8 I2C_channel, u8 addr, u8 pBuffer, u8 WriteAddr);


#endif

