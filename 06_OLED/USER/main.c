#include "stm32f10x.h"
#include "GPIOLIKE51.h"
#include "delay.h"
#include "LCD1602.h"
#include "myiic.h"
#include "oled.h"

#define Enable_12864 1

u8 *message_1 = "Fresh";

//=============================================================================
//文件名称：main
//功能概要：主函数
//参数说明：无
//函数返回：int
//=============================================================================
int main(void)
{
	u16 a_num=1222;
	delay_init();
	IIC_Init(2);
	delay_ms(100);
	OLED_Init();
	OLED_Clear();
	if(Enable_12864)
		LCD12864_Init();
	delay_ms(500);
	OLED_DrawBMP();
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);
	delay_ms(500);

	while (1)
	{
		OLED_Clear();
		delay_ms(300);
		if(Enable_12864)
		{
			gotoxy(1, 3);
			LCD_Write_String("OLED Demo");
			gotoxy(2, 2);
			LCD_Write_String("开发板Ver 1.1");
			gotoxy(3, 3);
			LCD_Write_String("--------");
			gotoxy(4, 3);
			LCD_Write_String("by  紫寒");
		}
		// Show a fixed string
		OLED_ShowString(34, 6, (u8 *)("By Zihan"), 16);
		// Show a variable type string
		OLED_ShowString(24, 3, message_1, 16);
		// Show a number
		OLED_ShowNum(72, 3, a_num, 4);
		// Show other text
		OLED_ShowCN(16, 0, 4);
		OLED_ShowCN(32, 0, 5);
		OLED_ShowCN(48, 0, 6);
		OLED_ShowCN(64, 0, 7);
		OLED_ShowCN(80, 0, 8);
		OLED_ShowCN(96, 0, 9);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		delay_ms(500);
		a_num += 1;
	}
}

