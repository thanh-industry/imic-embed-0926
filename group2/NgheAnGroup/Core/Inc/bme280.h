#ifndef BME280_H
#define BME280_H

#include "stm32l0xx_hal.h"

HAL_StatusTypeDef BME280_Init(void);
HAL_StatusTypeDef BME280_ReadTemp_x10(int32_t *t_x10);  // 282 = 28.2 C
uint8_t BME280_GetChipId(void);                         // 0x58 = BMP280, 0x60 = BME280

#endif
