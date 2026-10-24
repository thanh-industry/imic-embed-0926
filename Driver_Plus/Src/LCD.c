#include "LCD.h"


void LCD_Send_Cmd(uint8_t cmd)
{
	uint8_t data_high, data_low;
	uint8_t data_t[4];

	data_high = cmd & 0xF0;
	data_low  = (cmd << 4) & 0xF0;
	data_t[0] = data_high | LCD_EN | LCD_BACKLIGHT ;
	data_t[1] = data_high | LCD_BACKLIGHT;
	data_t[2] = data_low  | LCD_EN |LCD_BACKLIGHT;
	data_t[3] = data_low  | LCD_BACKLIGHT;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
	HAL_Delay(50);
}
void LCD_Send_Data(uint8_t data)
{
	uint8_t data_high, data_low;
	uint8_t data_t[4];

	data_high = data & 0xF0;
	data_low = (data << 4) & 0xF0;
	data_t[0] = data_high | LCD_EN | LCD_BACKLIGHT | LCD_RS;
	data_t[1] = data_high | LCD_BACKLIGHT | LCD_RS;
	data_t[2] = data_low | LCD_EN | LCD_BACKLIGHT | LCD_RS;
	data_t[3] = data_low | LCD_BACKLIGHT | LCD_RS;

	HAL_I2C_Master_Transmit(&hi2c1, LCD_ADDR, data_t, 4, 100);
	HAL_Delay(10);
}
void LCD_Init() {
	HAL_Delay(50);          // Cho dien ap on dinh sau khi cap nguon

	// 1. Chuoi Reset ep ve che do 4-bit
	LCD_Send_Cmd(0x30);
	HAL_Delay(10);

	// 2. Chuyen sang giao dien 4-bit
	LCD_Send_Cmd(0x20);
	HAL_Delay(10);

	// 3. Cau hinh hien thi
	LCD_Send_Cmd(0x28);     // Che do 4-bit, hien thi 2 dong, font 5x8
	LCD_Send_Cmd(0x0C);     // Bat man hinh, tat con tro
	LCD_Send_Cmd(0x06);     // Tu dong tang con tro sang phai
	LCD_Send_Cmd(0x01);     // Xoa man hinh
	HAL_Delay(3);           // Lenh Clear can thoi gian xu ly > 2ms
}
void Send_String(char *str)
{
	while(*str)
	{
		LCD_Send_Data(*str++);
	}
}
void LCD_Offset_Pos(uint8_t Row, uint8_t Colum)
{
	if (Row == 1)       LCD_Send_Cmd(0x80 + Colum);
	else if (Row == 2)  LCD_Send_Cmd(0xC0 + Colum);
}
