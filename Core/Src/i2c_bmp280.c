#include "i2c_bmp280.h"
#include "logging.h"

static I2C_HandleTypeDef *bmp_i2c = NULL;
static BMP280_CalibData_t calib;
static uint8_t bmp_addr = 0x76;

#define BMP280_REG_ID        0xD0
#define BMP280_REG_CALIB     0x88
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5
#define BMP280_REG_TEMP_MSB  0xFA

static void BMP280_WriteReg(uint8_t reg, uint8_t value) {
    uint8_t data[2] = {reg, value};
    HAL_I2C_Master_Transmit(bmp_i2c, bmp_addr << 1, data, 2, 50);
}

void BMP280_Init(I2C_HandleTypeDef *hi2c) {
    bmp_i2c = hi2c;
    uint8_t cmd;
    uint8_t calib_data[6];

    // Dò địa chỉ
    cmd = BMP280_REG_ID;
    if (HAL_I2C_Master_Transmit(bmp_i2c, 0x76 << 1, &cmd, 1, 50) == HAL_OK) bmp_addr = 0x76;
    else if (HAL_I2C_Master_Transmit(bmp_i2c, 0x77 << 1, &cmd, 1, 50) == HAL_OK) bmp_addr = 0x77;
    else { LOG_E("[BMP280] Khong tim thay!"); return; }


    // Đọc Calib
    cmd = BMP280_REG_CALIB;
    HAL_I2C_Master_Transmit(bmp_i2c, bmp_addr << 1, &cmd, 1, 50);
    HAL_I2C_Master_Receive(bmp_i2c, bmp_addr << 1, calib_data, 6, 50);

    calib.dig_T1 = (uint16_t)((calib_data[1] << 8) | calib_data[0]);
    calib.dig_T2 = (int16_t)((calib_data[3] << 8) | calib_data[2]);
    calib.dig_T3 = (int16_t)((calib_data[5] << 8) | calib_data[4]);

    // Cấu hình (Không dùng Reset 0xB6)
    BMP280_WriteReg(BMP280_REG_CONFIG, 0x00);
    BMP280_WriteReg(BMP280_REG_CTRL_MEAS, 0x27);
}

float BMP280_ReadTemperature(void) {
    uint8_t cmd = BMP280_REG_TEMP_MSB;
    uint8_t data[3];

    HAL_I2C_Master_Transmit(bmp_i2c, bmp_addr << 1, &cmd, 1, 50);
    if (HAL_I2C_Master_Receive(bmp_i2c, bmp_addr << 1, data, 3, 50) != HAL_OK) return -999.0f;

    int32_t adc_T = (int32_t)(((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4));

    int32_t var1, var2, T;
    var1 = ((((adc_T >> 3) - ((int32_t)calib.dig_T1 << 1))) * ((int32_t)calib.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((int32_t)calib.dig_T1)) * ((adc_T >> 4) - ((int32_t)calib.dig_T1))) >> 12) * ((int32_t)calib.dig_T3)) >> 14;

    int32_t t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;
    return (float)T / 100.0f;
}
