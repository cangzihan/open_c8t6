#include "LCD1602.h"
#include "delay.h"

#define use_1602 0 // Use 1602 or 12864

void write_IO_D(u8 cmd)
{
	if(cmd&0x01)
		D0 = 1;
	else
		D0 = 0;
	
	if(cmd&0x02)
		D1 = 1;
	else
		D1 = 0;
	
	if(cmd&0x04)
		D2 = 1;
	else
		D2 = 0;
	
	if(cmd&0x08)
		D3 = 1;
	else
		D3 = 0;
	
	if(cmd&0x10)
		D4 = 1;
	else
		D4 = 0;
	
	if(cmd&0x20)
		D5 = 1;
	else
		D5 = 0;
	
	if(cmd&0x40)
		D6 = 1;
	else
		D6 = 0;
	
	if(cmd&0x80)
		D7 = 1;
	else
		D7 = 0;
}

void LCD_Write_Com(uchar com)//写指令函数
{
	RS=0;
	RW=0;
	EN=1;
	delay_us(1);
	write_IO_D(com);
	delay_ms(1);
	EN=0;	
}
void LCD_Write_date(uchar date)//写数据函数
{
	RS=1;
	RW=0;
	EN=1;
	delay_us(1);
	write_IO_D(date);
	delay_ms(1);
	EN=0;
}

#if use_1602
void gotoxy(uchar x,uchar y)//是在x行，y列显示数据
{
	if(x==1)
	{
	   LCD_Write_Com(0x80+y-1);
	}
	if(x==2)
	{
		LCD_Write_Com(0xC0+y-1);
	}	
}
#else
void gotoxy(unsigned char x,unsigned char y)
{
	if(x==1)
	   LCD_Write_Com(0x80+y-1);
	else if(x==2)
	   LCD_Write_Com(0x90+y-1);
	else if(x==3)
		LCD_Write_Com(0x88+y-1);
	else if(x==4)
		LCD_Write_Com(0x98+y-1);
	delay_ms(1);	
}

#endif 

void LCD_Write_String(uchar *p)
{
	while(*p!=0)
	{
		LCD_Write_date(*p);	
		p++;
		delay_ms(1);	
	}
}

void LCD_ShowxNum1602(u16 x,u16 y,u32 num,u8 len)
{
	u8 i=0;
	gotoxy(x, y);
	while(i<len)
	{
		if(i)
			LCD_Write_date(num/(10*i)%10+'0');
		else
			LCD_Write_date(num%10+'0');
		i++;
	}
}

void LCD_Init1602(void)
{
	LCD_Write_Com(0x38);
	delay_ms(5);
	LCD_Write_Com(0x38);
	delay_ms(5);
	LCD_Write_Com(0x38);
	delay_ms(5);
	LCD_Write_Com(0x38);
	delay_ms(5);
	LCD_Write_Com(0x08);
	LCD_Write_Com(0x01);
	LCD_Write_Com(0x06);
	LCD_Write_Com(0x0c);
}

void LCD_Init12864(void)
{
	LCD_Write_Com(0x30);//功能设定
	delay_ms(5);
	LCD_Write_Com(0x0c);//显示状态开关
	delay_ms(5);
	LCD_Write_Com(0x01);//清屏
}

void LCD1602_Init(void)
{
	 GPIO_InitTypeDef  GPIO_InitStructure;
		
	 RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
		
	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_8;
	 GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	 GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	 GPIO_Init(GPIOA, &GPIO_InitStructure);
	 GPIO_ResetBits(GPIOA, GPIO_Pin_8|GPIO_Pin_12|GPIO_Pin_11);						 //输出高

	 GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	 GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	 GPIO_ResetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14); 						 //输出高 

	LCD_Init1602();
}

void LCD12864_Init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;
		
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA|RCC_APB2Periph_GPIOB, ENABLE);	
		
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_11|GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 		 //推挽输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;		 //IO口速度为50MHz
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_ResetBits(GPIOA, GPIO_Pin_8|GPIO_Pin_12|GPIO_Pin_11);						 //输出高

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);	  				 //推挽输出 ，IO口速度为50MHz
	GPIO_ResetBits(GPIOB, GPIO_Pin_3|GPIO_Pin_4|GPIO_Pin_5|GPIO_Pin_6|GPIO_Pin_7|GPIO_Pin_8|GPIO_Pin_9|GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14); 		//输出高 

	LCD_Init12864();
}
