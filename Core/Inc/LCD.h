#ifndef LCD_H
#define LCD_H
#include "main.h"
#define LCD_ADDR       (0x27 << 1)
#define LCD_BACKLIGHT   0x08
#define LCD_RS          0x01
#define LCD_EN          0x04

extern I2C_HandleTypeDef hi2c1;
void LCD_Send_Cmd(uint8_t cmd);
void LCD_Send_Data(uint8_t data);
void LCD_Init();
void Send_String(char *str);

#define Clear_Dislay     LCD_Send_Cmd(0x01)
#define Return_Home      LCD_Send_Cmd(0x02)
#define Entry_Mode_Set   LCD_Send_Cmd(0x06)

#define LCD_Row_1        LCD_Send_Cmd(0x80)
#define LCD_Row_2        LCD_Send_Cmd(0xC0)
#define LCD_Row_3        LCD_Send_Cmd(0x94)
#define LCD_Row_4        LCD_Send_Cmd(0xD4)

#endif
