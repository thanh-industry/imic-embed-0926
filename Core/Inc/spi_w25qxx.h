/* spi_w25qxx.h */
#ifndef __SPI_W25QXX_H
#define __SPI_W25QXX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* Lệnh W25QXX */
#define W25Q_JEDEC_ID         0x9F
#define W25Q_READ_STATUS1     0x05
#define W25Q_WRITE_ENABLE     0x06
#define W25Q_READ_DATA        0x03
#define W25Q_PAGE_PROGRAM     0x02
#define W25Q_SECTOR_ERASE     0x20
#define W25Q_CHIP_ERASE       0xC7

/* Thông số cấu hình */
#define W25Q_SECTOR_SIZE      4096
#define W25Q_PAGE_SIZE        256


/* ID */
#define W25Q32_ID             0xEF4016

/* Nguyên mẫu hàm */
void W25Q_Init(SPI_HandleTypeDef *hspi);
uint32_t W25Q_ReadID(void);
uint8_t  W25Q_ReadStatus(void);
void     W25Q_WaitForReady(void);
void W25Q_ReadData(uint32_t addr, uint8_t *pData, uint32_t len);
void W25Q_WriteData(uint32_t addr, uint8_t *pData, uint32_t len);
void W25Q_EraseSector(uint32_t sector_addr);
void W25Q_EraseChip(void);

#ifdef __cplusplus
}
#endif

#endif /* __SPI_W25QXX_H */
