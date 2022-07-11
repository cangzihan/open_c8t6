#ifndef __MYIIC_H
#define __MYIIC_H
#include "sys.h"

#define USE_LIB 1

#if USE_LIB
 
/* STM32 I2C Fast Mode */
#define I2C_Speed 200000 //* Disable some hardware, this speed could be set to 400k

/* �����ַֻҪ�� STM32 ��ҵ� I2C ������ַ��һ������ */
#define I2Cx_OWN_ADDRESS7 0X0A

/*�ȴ���ʱʱ��*/
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

//IO��������
 
#define SDA_IN()  {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)8<<12;}
#define SDA_OUT() {GPIOB->CRH&=0XFFFF0FFF;GPIOB->CRH|=(u32)3<<12;}

//IO��������	 
#define IIC_SCL    PBout(10) //SCL
#define IIC_SDA    PBout(11) //SDA	 
#define READ_SDA   PBin(11)  //����SDA

void IIC_Start(void);				//����IIC��ʼ�ź�
void IIC_Stop(void);	  		//����IICֹͣ�ź�
void IIC_Send_Byte(u8 txd);	//IIC����һ���ֽ�
void IIC_Ack(void);					//IIC����ACK�ź�
void IIC_NAck(void);				//IIC������ACK�ź� 
u8 IIC_Read_Byte(unsigned char ack);//IIC��ȡһ���ֽ�

#endif

//IIC���в�������
void IIC_Init(u8 I2C_channel);           //��ʼ��IIC��IO��				
u8 IIC_Wait_Ack(u8 I2C_channel, uint32_t I2C_EVENT); 	  //IIC�ȴ�ACK�ź�

static uint32_t I2C_TIMEOUT_UserCallback(uint32_t errorCode);
static void I2C_GPIO_Config(u8 I2C_channel);
u8 I2C_ByteWrite(u8 I2C_channel, u8 addr, u8 pBuffer, u8 WriteAddr);


#endif

