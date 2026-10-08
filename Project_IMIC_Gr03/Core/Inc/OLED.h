
#ifndef __OLED_H__
#define __OLED_H__

#include "main.h"

extern I2C_HandleTypeDef hi2c1;
// dia chi cua man OLED
#define OLED_BASE_ADDR  (0x3C << 1)

// Kick thuoc man hinh
#define OLED_WIDTH    128 // 128 SEG  la 128 cot
#define OLED_HEIGHT   64  // 64/8 = 8 page la 8 bit theo hang doc

// gui command va data
#define OLED_CMD    0x00
#define OLED_DATA   0x40

// Khai bao cac ham ngoai vi
void OLED_Init();
void OLED_Clear();
void OLED_Update_Screen(void);
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color);
void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color);
#endif
