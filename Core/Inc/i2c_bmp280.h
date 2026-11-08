#ifndef __I2C_BMP280_H
#define __I2C_BMP280_H

#include "main.h"

/* Địa chỉ I2C của BMP280.
 * Nếu module có chân SDO nối GND -> Địa chỉ 0x76
 * Nếu module có chân SDO nối VCC -> Địa chỉ 0x77 */
#define BMP280_ADDR  (0x76 << 1)

/* Cấu trúc lưu các thông số hiệu chuẩn (Calibration) của chip */
typedef struct {
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;
} BMP280_CalibData_t;

/* Khai báo các hàm Driver */
void BMP280_Init(I2C_HandleTypeDef *hi2c);
float BMP280_ReadTemperature(void);

#endif
