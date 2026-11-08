/* spi_w25qxx.c */
#include "spi_w25qxx.h"

static SPI_HandleTypeDef *w25q_spi = NULL;

/* Chọn chip (CS = 0) */
static void W25Q_Select(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_RESET);
}

/* Bỏ chọn chip (CS = 1) */
static void W25Q_Deselect(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_SET);
}

void W25Q_Init(SPI_HandleTypeDef *hspi) {
    w25q_spi = hspi;
    W25Q_Deselect();
}

uint32_t W25Q_ReadID(void) {
    uint8_t tx[4] = {W25Q_JEDEC_ID, 0, 0, 0};
    uint8_t rx[4] = {0};

    W25Q_Select();
    HAL_SPI_TransmitReceive(w25q_spi, tx, rx, 4, 100);
    W25Q_Deselect();

    return ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | rx[3];
}

uint8_t W25Q_ReadStatus(void) {
    uint8_t tx[2] = {W25Q_READ_STATUS1, 0};
    uint8_t rx[2] = {0};

    W25Q_Select();
    HAL_SPI_TransmitReceive(w25q_spi, tx, rx, 2, 100);
    W25Q_Deselect();

    return rx[1];
}

void W25Q_WaitForReady(void) {
    while ((W25Q_ReadStatus() & 0x01) == 0x01);
}

void W25Q_ReadData(uint32_t addr, uint8_t *pData, uint32_t len) {
    uint8_t tx[4] = {W25Q_READ_DATA, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx, 4, 100);
    HAL_SPI_Receive(w25q_spi, pData, len, 100);
    W25Q_Deselect();
}

void W25Q_WriteData(uint32_t addr, uint8_t *pData, uint32_t len) {
    if (len > W25Q_PAGE_SIZE) len = W25Q_PAGE_SIZE;

    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    uint8_t tx_header[4] = {W25Q_PAGE_PROGRAM, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx_header, 4, 100);
    HAL_SPI_Transmit(w25q_spi, pData, len, 100);
    W25Q_Deselect();

    W25Q_WaitForReady();
}

void W25Q_EraseSector(uint32_t sector_addr) {
    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    uint8_t tx_header[4] = {W25Q_SECTOR_ERASE, (uint8_t)(sector_addr >> 16), (uint8_t)(sector_addr >> 8), (uint8_t)sector_addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx_header, 4, 100);
    W25Q_Deselect();

    W25Q_WaitForReady();
}

void W25Q_EraseChip(void) {
    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    uint8_t cmd_erase = W25Q_CHIP_ERASE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_erase, 1, 100);
    W25Q_Deselect();

    W25Q_WaitForReady();
}
