#ifndef __I2C_OLED_LCD_H
#define __I2C_OLED_LCD_H

#include "main.h"

/* Địa chỉ I2C của màn hình OLED SSD1306 thường là 0x3C */
#define OLED_ADDR (0x3C << 1)

/* Khai báo các hàm điều khiển màn hình */
void OLED_Init(I2C_HandleTypeDef *hi2c);
void OLED_Clear(void);
void OLED_ShowString(uint8_t x, uint8_t y, char* str);
void OLED_UpdateScreen(void);

#endif
