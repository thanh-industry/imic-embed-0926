/* i2c_bmp280.c */
#include "i2c_bmp280.h"
#include "logging.h"

#define BMP280_ADDR_PRIMARY    0x76U
#define BMP280_ADDR_SECONDARY  0x77U
#define BMP280_I2C_TIMEOUT_MS  50U
#define BMP280_ERR_TEMP        (-999.0f)

#define BMP280_REG_ID          0xD0U
#define BMP280_REG_CALIB       0x88U
#define BMP280_REG_CTRL_MEAS   0xF4U
#define BMP280_REG_CONFIG      0xF5U
#define BMP280_REG_TEMP_MSB    0xFAU

static I2C_HandleTypeDef  *s_bmp_i2c   = NULL;
static BMP280_CalibData_t  s_calib;
static uint8_t             s_bmp_addr  = BMP280_ADDR_PRIMARY;
static uint8_t             s_bmp_ready = 0U;

static HAL_StatusTypeDef BMP280_WriteReg(uint8_t reg, uint8_t value) {
    if (s_bmp_i2c == NULL) {
        return HAL_ERROR;
    }
    return HAL_I2C_Mem_Write(s_bmp_i2c, (uint16_t)(s_bmp_addr << 1),
                             reg, I2C_MEMADD_SIZE_8BIT,
                             &value, 1U, BMP280_I2C_TIMEOUT_MS);
}

void BMP280_Init(I2C_HandleTypeDef *hi2c) {
    uint8_t chip_id = 0U;
    uint8_t calib_data[6];

    s_bmp_i2c   = hi2c;
    s_bmp_ready = 0U;

    if (s_bmp_i2c == NULL) {
        LOG_E("[BMP280] Con tro I2C NULL!");
        return;
    }

    if (HAL_I2C_Mem_Read(s_bmp_i2c, (uint16_t)(BMP280_ADDR_PRIMARY << 1),
                         BMP280_REG_ID, I2C_MEMADD_SIZE_8BIT,
                         &chip_id, 1U, BMP280_I2C_TIMEOUT_MS) == HAL_OK) {
        s_bmp_addr = BMP280_ADDR_PRIMARY;
    } else if (HAL_I2C_Mem_Read(s_bmp_i2c, (uint16_t)(BMP280_ADDR_SECONDARY << 1),
                                BMP280_REG_ID, I2C_MEMADD_SIZE_8BIT,
                                &chip_id, 1U, BMP280_I2C_TIMEOUT_MS) == HAL_OK) {
        s_bmp_addr = BMP280_ADDR_SECONDARY;
    } else {
        LOG_E("[BMP280] Khong tim thay cam bien tren bus I2C!");
        return;
    }

    if (HAL_I2C_Mem_Read(s_bmp_i2c, (uint16_t)(s_bmp_addr << 1),
                         BMP280_REG_CALIB, I2C_MEMADD_SIZE_8BIT,
                         calib_data, 6U, BMP280_I2C_TIMEOUT_MS) != HAL_OK) {
        LOG_E("[BMP280] Loi doc du lieu calib!");
        return;
    }

    s_calib.dig_T1 = (uint16_t)(((uint16_t)calib_data[1] << 8) | calib_data[0]);
    s_calib.dig_T2 = (int16_t)(((uint16_t)calib_data[3] << 8) | calib_data[2]);
    s_calib.dig_T3 = (int16_t)(((uint16_t)calib_data[5] << 8) | calib_data[4]);

    if (s_calib.dig_T1 == 0U) {
        LOG_E("[BMP280] Du lieu calib khong hop le!");
        return;
    }

    if ((BMP280_WriteReg(BMP280_REG_CONFIG, 0x00U) != HAL_OK) ||
        (BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27U) != HAL_OK)) {
        LOG_E("[BMP280] Loi cau hinh thanh ghi!");
        return;
    }

    s_bmp_ready = 1U;
}

float BMP280_ReadTemperature(void) {
    uint8_t data[3];
    int32_t adc_T;
    int32_t var1;
    int32_t var2;
    int32_t t_fine;
    int32_t temp_fixed;

    if ((s_bmp_i2c == NULL) || (s_bmp_ready == 0U)) {
        return BMP280_ERR_TEMP;
    }

    if (HAL_I2C_Mem_Read(s_bmp_i2c, (uint16_t)(s_bmp_addr << 1),
                         BMP280_REG_TEMP_MSB, I2C_MEMADD_SIZE_8BIT,
                         data, 3U, BMP280_I2C_TIMEOUT_MS) != HAL_OK) {
        return BMP280_ERR_TEMP;
    }

    adc_T = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | ((uint32_t)data[2] >> 4));

    var1   = ((((adc_T >> 3) - ((int32_t)s_calib.dig_T1 << 1))) * ((int32_t)s_calib.dig_T2)) >> 11;
    var2   = (((((adc_T >> 4) - ((int32_t)s_calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)s_calib.dig_T1))) >> 12) * ((int32_t)s_calib.dig_T3)) >> 14;
    t_fine = var1 + var2;

    temp_fixed = (t_fine * 5 + 128) >> 8;
    return (float)temp_fixed / 100.0f;
}
