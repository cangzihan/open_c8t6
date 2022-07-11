#ifndef __OLED_H
#define __OLED_H
#include "sys.h"

void OLED_Init(void);
void OLED_Clear(void);
void OLED_ShowString(u8 x, u8 y, u8 *str, u8 chr_size);
void OLED_ShowCN(u8 x,u8 y,u8 index);
void OLED_DrawBMP(void);

void OLED_ShowNum(u8 x,u8 y,u16 num,u8 n);

#endif
















