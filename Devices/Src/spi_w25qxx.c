/* spi_w25qxx.c */
#include "spi_w25qxx.h"
#include "cmsis_os.h"

#define W25Q_SPI_TIMEOUT_MS       100U
#define W25Q_READY_TIMEOUT_MS     500U
#define W25Q_CHIP_ERASE_TIMEOUT   30000U
#define W25Q_STATUS_BUSY_MASK     0x01U

static SPI_HandleTypeDef *s_w25q_spi = NULL;

static void W25Q_Select(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_RESET);
}

static void W25Q_Deselect(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_SET);
}

static uint8_t W25Q_WaitForReadyTimeout(uint32_t timeout_ms) {
    uint32_t start_tick = HAL_GetTick();

    while ((W25Q_ReadStatus() & W25Q_STATUS_BUSY_MASK) != 0U) {
        if ((HAL_GetTick() - start_tick) >= timeout_ms) {
            return 0U;
        }
        if (osKernelGetState() == osKernelRunning) {
            (void)osDelay(1U);
        }
    }
    return 1U;
}

static HAL_StatusTypeDef W25Q_WriteEnable(void) {
    uint8_t           cmd_enable = W25Q_WRITE_ENABLE;
    HAL_StatusTypeDef status;

    if (s_w25q_spi == NULL) {
        return HAL_ERROR;
    }

    W25Q_Select();
    status = HAL_SPI_Transmit(s_w25q_spi, &cmd_enable, 1U, W25Q_SPI_TIMEOUT_MS);
    W25Q_Deselect();

    return status;
}

void W25Q_Init(SPI_HandleTypeDef *hspi) {
    s_w25q_spi = hspi;
    W25Q_Deselect();
}

uint32_t W25Q_ReadID(void) {
    uint8_t           tx[4] = { W25Q_JEDEC_ID, 0U, 0U, 0U };
    uint8_t           rx[4] = { 0U, 0U, 0U, 0U };
    HAL_StatusTypeDef status;

    if (s_w25q_spi == NULL) {
        return 0x000000UL;
    }

    W25Q_Select();
    status = HAL_SPI_TransmitReceive(s_w25q_spi, tx, rx, 4U, W25Q_SPI_TIMEOUT_MS);
    W25Q_Deselect();

    if (status != HAL_OK) {
        return 0x000000UL;
    }

    return ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | (uint32_t)rx[3];
}

uint8_t W25Q_ReadStatus(void) {
    uint8_t           tx[2] = { W25Q_READ_STATUS1, 0U };
    uint8_t           rx[2] = { 0U, 0xFFU };
    HAL_StatusTypeDef status;

    if (s_w25q_spi == NULL) {
        return 0xFFU;
    }

    W25Q_Select();
    status = HAL_SPI_TransmitReceive(s_w25q_spi, tx, rx, 2U, W25Q_SPI_TIMEOUT_MS);
    W25Q_Deselect();

    if (status != HAL_OK) {
        return 0xFFU;
    }

    return rx[1];
}

void W25Q_WaitForReady(void) {
    (void)W25Q_WaitForReadyTimeout(W25Q_READY_TIMEOUT_MS);
}

void W25Q_ReadData(uint32_t addr, uint8_t *pData, uint32_t len) {
    uint8_t tx[4];

    if ((s_w25q_spi == NULL) || (pData == NULL) || (len == 0UL)) {
        return;
    }

    tx[0] = W25Q_READ_DATA;
    tx[1] = (uint8_t)(addr >> 16);
    tx[2] = (uint8_t)(addr >> 8);
    tx[3] = (uint8_t)addr;

    W25Q_Select();
    if (HAL_SPI_Transmit(s_w25q_spi, tx, 4U, W25Q_SPI_TIMEOUT_MS) == HAL_OK) {
        (void)HAL_SPI_Receive(s_w25q_spi, pData, (uint16_t)len, W25Q_SPI_TIMEOUT_MS);
    }
    W25Q_Deselect();
}

void W25Q_WriteData(uint32_t addr, uint8_t *pData, uint32_t len) {
    uint8_t           tx_header[4];
    HAL_StatusTypeDef status;

    if ((s_w25q_spi == NULL) || (pData == NULL) || (len == 0UL)) {
        return;
    }

    if (len > W25Q_PAGE_SIZE) {
        len = W25Q_PAGE_SIZE;
    }

    if (W25Q_WriteEnable() != HAL_OK) {
        return;
    }

    tx_header[0] = W25Q_PAGE_PROGRAM;
    tx_header[1] = (uint8_t)(addr >> 16);
    tx_header[2] = (uint8_t)(addr >> 8);
    tx_header[3] = (uint8_t)addr;

    W25Q_Select();
    status = HAL_SPI_Transmit(s_w25q_spi, tx_header, 4U, W25Q_SPI_TIMEOUT_MS);
    if (status == HAL_OK) {
        status = HAL_SPI_Transmit(s_w25q_spi, pData, (uint16_t)len, W25Q_SPI_TIMEOUT_MS);
    }
    W25Q_Deselect();

    if (status == HAL_OK) {
        (void)W25Q_WaitForReadyTimeout(W25Q_READY_TIMEOUT_MS);
    }
}

void W25Q_EraseSector(uint32_t sector_addr) {
    uint8_t           tx_header[4];
    HAL_StatusTypeDef status;

    if (W25Q_WriteEnable() != HAL_OK) {
        return;
    }

    tx_header[0] = W25Q_SECTOR_ERASE;
    tx_header[1] = (uint8_t)(sector_addr >> 16);
    tx_header[2] = (uint8_t)(sector_addr >> 8);
    tx_header[3] = (uint8_t)sector_addr;

    W25Q_Select();
    status = HAL_SPI_Transmit(s_w25q_spi, tx_header, 4U, W25Q_SPI_TIMEOUT_MS);
    W25Q_Deselect();

    if (status == HAL_OK) {
        (void)W25Q_WaitForReadyTimeout(W25Q_READY_TIMEOUT_MS);
    }
}

void W25Q_EraseChip(void) {
    uint8_t           cmd_erase = W25Q_CHIP_ERASE;
    HAL_StatusTypeDef status;

    if (W25Q_WriteEnable() != HAL_OK) {
        return;
    }

    W25Q_Select();
    status = HAL_SPI_Transmit(s_w25q_spi, &cmd_erase, 1U, W25Q_SPI_TIMEOUT_MS);
    W25Q_Deselect();

    if (status == HAL_OK) {
        (void)W25Q_WaitForReadyTimeout(W25Q_CHIP_ERASE_TIMEOUT);
    }
}
