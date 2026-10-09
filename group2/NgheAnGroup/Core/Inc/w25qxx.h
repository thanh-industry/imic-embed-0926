#ifndef W25QXX_H
#define W25QXX_H

#include "main.h"
#include <stdint.h>

HAL_StatusTypeDef W25_Init(SPI_HandleTypeDef *hspi);
HAL_StatusTypeDef W25_ReadId(uint8_t *mfr, uint16_t *dev);
HAL_StatusTypeDef W25_Read(uint32_t addr, uint8_t *buf, uint16_t len);
HAL_StatusTypeDef W25_EraseSector(uint32_t addr);               /* xóa 4 KB */
HAL_StatusTypeDef W25_PageProgram(uint32_t addr, const uint8_t *buf, uint16_t len); /* len <= 256, không vượt biên page */
HAL_StatusTypeDef W25_SelfTest(void);                           /* ghi rồi đọc lại */

#endif
