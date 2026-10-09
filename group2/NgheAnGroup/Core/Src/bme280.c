#include "bme280.h"
#include "i2c.h"

static uint8_t  bme_addr = (0x76 << 1);
static uint8_t  chip_id;
static uint16_t T1;
static int16_t  T2, T3;

static HAL_StatusTypeDef rd(uint8_t reg, uint8_t *b, uint16_t n)
{
    return HAL_I2C_Mem_Read(&hi2c1, bme_addr, reg, I2C_MEMADD_SIZE_8BIT, b, n, 50);
}

uint8_t BME280_GetChipId(void) { return chip_id; }

HAL_StatusTypeDef BME280_Init(void)
{
    uint8_t cal[6], v;

    if (HAL_I2C_IsDeviceReady(&hi2c1, (0x76 << 1), 2, 20) == HAL_OK)      bme_addr = (0x76 << 1);
    else if (HAL_I2C_IsDeviceReady(&hi2c1, (0x77 << 1), 2, 20) == HAL_OK) bme_addr = (0x77 << 1);
    else return HAL_ERROR;

    if (rd(0xD0, &chip_id, 1) != HAL_OK) return HAL_ERROR;
    if (chip_id != 0x60 && chip_id != 0x58) return HAL_ERROR;

    if (rd(0x88, cal, 6) != HAL_OK) return HAL_ERROR;
    T1 = (uint16_t)(cal[1] << 8 | cal[0]);
    T2 = (int16_t)(cal[3] << 8 | cal[2]);
    T3 = (int16_t)(cal[5] << 8 | cal[4]);

    v = 0x23;  // osrs_t x1, bỏ qua áp suất, normal mode
    return HAL_I2C_Mem_Write(&hi2c1, bme_addr, 0xF4, I2C_MEMADD_SIZE_8BIT, &v, 1, 50);
}

HAL_StatusTypeDef BME280_ReadTemp_x10(int32_t *t_x10)
{
    uint8_t d[3];
    if (rd(0xFA, d, 3) != HAL_OK) return HAL_ERROR;

    int32_t adc = ((int32_t)d[0] << 12) | ((int32_t)d[1] << 4) | (d[2] >> 4);
    if (adc == 0x80000) return HAL_ERROR;   // chưa có kết quả đo

    int32_t v1 = (((adc >> 3) - ((int32_t)T1 << 1)) * T2) >> 11;
    int32_t v2 = (((((adc >> 4) - T1) * ((adc >> 4) - T1)) >> 12) * T3) >> 14;
    int32_t t_fine = v1 + v2;
    int32_t t100 = (t_fine * 5 + 128) >> 8;   // 0.01 C

    *t_x10 = t100 / 10;
    return HAL_OK;
}
