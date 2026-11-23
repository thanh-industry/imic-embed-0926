#ifndef __I2C_OLED_LCD_H
#define __I2C_OLED_LCD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

#define OLED_ADDR (0x3CU << 1)

void OLED_Init(I2C_HandleTypeDef *hi2c);
void OLED_Clear(void);
void OLED_ShowString(uint8_t x, uint8_t y, const char *str);
void OLED_UpdateScreen(void);

#ifdef __cplusplus
}
#endif

#endif /* __I2C_OLED_LCD_H */
