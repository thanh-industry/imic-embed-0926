#include "w25qxx.h"
#include <string.h>

#define CMD_WREN   0x06
#define CMD_RDSR1  0x05
#define CMD_READ   0x03
#define CMD_PP     0x02
#define CMD_SE     0x20
#define CMD_JEDEC  0x9F

static SPI_HandleTypeDef *w25_spi;

static inline void cs_low(void)  { HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_RESET); }
static inline void cs_high(void) { HAL_GPIO_WritePin(W25_CS_GPIO_Port, W25_CS_Pin, GPIO_PIN_SET); }

static HAL_StatusTypeDef write_enable(void)
{
    uint8_t c = CMD_WREN;
    cs_low();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(w25_spi, &c, 1, 100);
    cs_high();
    return st;
}

static HAL_StatusTypeDef wait_busy(uint32_t timeout_ms)
{
    uint8_t cmd = CMD_RDSR1, sr = 0xFF;
    uint32_t t0 = HAL_GetTick();
    do {
        cs_low();
        HAL_SPI_Transmit(w25_spi, &cmd, 1, 100);
        HAL_SPI_Receive(w25_spi, &sr, 1, 100);
        cs_high();
        if (!(sr & 0x01)) return HAL_OK;     /* bit BUSY = 0 */
        HAL_Delay(1);
    } while (HAL_GetTick() - t0 < timeout_ms);
    return HAL_TIMEOUT;
}

static void send_cmd_addr(uint8_t cmd, uint32_t addr, uint8_t *out4)
{
    out4[0] = cmd;
    out4[1] = (addr >> 16) & 0xFF;
    out4[2] = (addr >> 8) & 0xFF;
    out4[3] = addr & 0xFF;
}

HAL_StatusTypeDef W25_ReadId(uint8_t *mfr, uint16_t *dev)
{
    uint8_t cmd = CMD_JEDEC, id[3] = {0};
    cs_low();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(w25_spi, &cmd, 1, 100);
    if (st == HAL_OK) st = HAL_SPI_Receive(w25_spi, id, 3, 100);
    cs_high();
    *mfr = id[0];
    *dev = (uint16_t)(id[1] << 8 | id[2]);
    return st;
}

HAL_StatusTypeDef W25_Init(SPI_HandleTypeDef *hspi)
{
    uint8_t mfr; uint16_t dev;
    w25_spi = hspi;
    cs_high();
    HAL_Delay(5);
    if (W25_ReadId(&mfr, &dev) != HAL_OK) return HAL_ERROR;
    /* Chấp nhận mọi hãng, chỉ loại trường hợp không có chip */
    if (mfr == 0x00 || mfr == 0xFF) return HAL_ERROR;
    return HAL_OK;
}

HAL_StatusTypeDef W25_Read(uint32_t addr, uint8_t *buf, uint16_t len)
{
    uint8_t h[4];
    send_cmd_addr(CMD_READ, addr, h);
    cs_low();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(w25_spi, h, 4, 100);
    if (st == HAL_OK) st = HAL_SPI_Receive(w25_spi, buf, len, 200);
    cs_high();
    return st;
}

HAL_StatusTypeDef W25_EraseSector(uint32_t addr)
{
    uint8_t h[4];
    if (write_enable() != HAL_OK) return HAL_ERROR;
    send_cmd_addr(CMD_SE, addr, h);
    cs_low();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(w25_spi, h, 4, 100);
    cs_high();
    if (st != HAL_OK) return st;
    return wait_busy(500);
}

HAL_StatusTypeDef W25_PageProgram(uint32_t addr, const uint8_t *buf, uint16_t len)
{
    uint8_t h[4];
    if (write_enable() != HAL_OK) return HAL_ERROR;
    send_cmd_addr(CMD_PP, addr, h);
    cs_low();
    HAL_StatusTypeDef st = HAL_SPI_Transmit(w25_spi, h, 4, 100);
    if (st == HAL_OK) st = HAL_SPI_Transmit(w25_spi, (uint8_t *)buf, len, 200);
    cs_high();
    if (st != HAL_OK) return st;
    return wait_busy(10);
}

HAL_StatusTypeDef W25_SelfTest(void)
{
    const uint32_t addr = 0x001000;               /* sector thử nghiệm, sẽ bị xóa */
    const uint8_t pat[8] = {'S','T','M','3','2','L','0','!'};
    uint8_t rd[8] = {0};

    if (W25_EraseSector(addr) != HAL_OK) return HAL_ERROR;
    if (W25_PageProgram(addr, pat, sizeof(pat)) != HAL_OK) return HAL_ERROR;
    if (W25_Read(addr, rd, sizeof(rd)) != HAL_OK) return HAL_ERROR;
    return (memcmp(pat, rd, sizeof(pat)) == 0) ? HAL_OK : HAL_ERROR;
}
