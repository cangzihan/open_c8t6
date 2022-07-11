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

	/* ʹ���� I2C �йص�ʱ�� */
	if(I2C_channel==1)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	else if(I2C_channel==2)
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);

	/* I2C_SCL��I2C_SDA*/
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_OD; // ��©���
	GPIO_Init(GPIOB, &GPIO_InitStructure);
 
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
#else
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);	//ʹ��GPIOBʱ��
	
	if(I2C_channel==1)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
	else if(I2C_channel==2)
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10|GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP ;   //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	if(I2C_channel==1)
		GPIO_SetBits(GPIOB,GPIO_Pin_6|GPIO_Pin_7); 	  //PB6,PB7 �����
	else if(I2C_channel==2)
		GPIO_SetBits(GPIOB,GPIO_Pin_10|GPIO_Pin_11); 	//PB10,PB11 �����
#endif
}

//��ʼ��IIC
void IIC_Init(u8 I2C_channel)
{
	I2C_GPIO_Config(I2C_channel);
#if USE_LIB
	I2C_Mode_Config(I2C_channel);	
#endif
}

//�ȴ�Ӧ���źŵ���
//����ֵ��1������Ӧ��ʧ��
//        0������Ӧ��ɹ�
u8 IIC_Wait_Ack(u8 I2C_channel, uint32_t I2C_EVENT)
{
#if USE_LIB
	I2C_TypeDef* I2Cx;
	__IO uint32_t I2C_time = I2CT_FLAG_TIMEOUT;
	/* ��� EV6 �¼��������־*/
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
	SDA_IN();      //SDA����Ϊ����  
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
	IIC_SCL=0;//ʱ�����0 	   
#endif 
	return 0;
}


#if USE_LIB

static void I2C_Mode_Config(u8 I2C_channel)
{
	I2C_InitTypeDef I2C_InitStructure;
 
	/* I2C ���� */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
 
	/* �ߵ�ƽ�����ȶ����͵�ƽ���ݱ仯 SCL ʱ���ߵ�ռ�ձ� */
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
 
	I2C_InitStructure.I2C_OwnAddress1 =I2Cx_OWN_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable ;
 
	/* I2C ��Ѱַģʽ */
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
 
	/* ͨ������ */
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;
 
	if(I2C_channel==1)
	{
		/* I2C ��ʼ�� */
		I2C_Init(I2C1, &I2C_InitStructure);
	 
		/* ʹ�� I2C */
		I2C_Cmd(I2C1, ENABLE);
	}
	else if(I2C_channel==2)
	{
		/* I2C ��ʼ�� */
		I2C_Init(I2C2, &I2C_InitStructure);
	 
		/* ʹ�� I2C */
		I2C_Cmd(I2C2, ENABLE);
	}
}

/**
* @brief I2C �ȴ��¼���ʱ������»�����������������
* @param errorCode��������룬����������λ���ĸ����ڳ���.
* @retval ���� 0����ʾ IIC ��ȡʧ��.
*/
static uint32_t I2C_TIMEOUT_UserCallback(uint32_t errorCode)
{
//	/* ʹ�ô��� printf ���������Ϣ��������� */
//	EEPROM_ERROR("I2C �ȴ���ʱ!errorCode = %d",errorCode);
	return 0;
}

#else

//����IIC��ʼ�ź�
void IIC_Start(void)
{
	SDA_OUT();     //sda�����
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	delay_us(4);
	IIC_SDA=0;//START:when CLK is high,DATA change form high to low 
	delay_us(4);
	IIC_SCL=0;//ǯסI2C���ߣ�׼�����ͻ�������� 
}

//����IICֹͣ�ź�
void IIC_Stop(void)
{
	SDA_OUT();//sda�����
	IIC_SCL=0;
	IIC_SDA=0;//STOP:when CLK is high DATA change form low to high
	delay_us(4);
	IIC_SCL=1; 
	IIC_SDA=1;//����I2C���߽����ź�
	delay_us(4);		
}

//����ACKӦ��
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

//������ACKӦ��		    
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

//IIC����һ���ֽ�
//���شӻ�����Ӧ��
//1����Ӧ��
//0����Ӧ��			  
void IIC_Send_Byte(u8 txd)
{
  u8 t;   
	SDA_OUT(); 	    
  IIC_SCL=0;//����ʱ�ӿ�ʼ���ݴ���
  for(t=0;t<8;t++)
  {            
    IIC_SDA=(txd&0x80)>>7;
    txd<<=1; 	  
		delay_us(2);   //��TEA5767��������ʱ���Ǳ����
		IIC_SCL=1;
		delay_us(2); 
		IIC_SCL=0;	
		delay_us(2);
  }
}

//��1���ֽڣ�ack=1ʱ������ACK��ack=0������nACK   
u8 IIC_Read_Byte(unsigned char ack)
{
	unsigned char i,receive=0;
	SDA_IN();//SDA����Ϊ����
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
		IIC_NAck();//����nACK
   else
		IIC_Ack(); //����ACK   
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
	
	/* ���� I2C ��ʼ�ź� */
	I2C_GenerateSTART(I2Cx, ENABLE);
	/* ��� EV5 �¼��������־*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_MODE_SELECT);

	/* ���� EEPROM �豸��ַ */
	I2C_Send7bitAddress(I2Cx, addr, I2C_Direction_Transmitter);
	/* ��� EV6 �¼��������־*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED);
 
	/* ����Ҫд��� EEPROM �ڲ���ַ(�� EEPROM �ڲ��洢���ĵ�ַ) */
	I2C_SendData(I2Cx, WriteAddr);
	/* ��� EV8 �¼��������־*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
	
	/* ����һ�ֽ�Ҫд������� */
	I2C_SendData(I2Cx, pBuffer);
	/* ��� EV8 �¼��������־*/
	IIC_Wait_Ack(I2C_channel, I2C_EVENT_MASTER_BYTE_TRANSMITTED);
 
	/* ����ֹͣ�ź� */
	I2C_GenerateSTOP(I2Cx, ENABLE); 
#else
		IIC_Start(); //��ʼ�ź�
		IIC_Send_Byte(addr);//д������ַ
		IIC_Wait_Ack(2, 0);
		IIC_Send_Byte(WriteAddr);//�ڲ��Ĵ�����ַ
		IIC_Wait_Ack(2, 0);
		IIC_Send_Byte(pBuffer);//�ڲ��Ĵ�������
		IIC_Wait_Ack(2, 0);
		IIC_Stop();
#endif 
	return 1;
}
