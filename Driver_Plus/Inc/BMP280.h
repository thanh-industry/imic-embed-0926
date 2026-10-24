#ifndef BMP280_H
#define BMP280_H
#include "main.h"

extern SPI_HandleTypeDef hspi1;

#define PORT_CS  GPIOA
#define CS       GPIO_PIN_4
#define LOW      GPIO_PIN_RESET
#define HIGH     GPIO_PIN_SET

#define BMP_REG_CALIB         0x88
#define BMP_REG_ID            0xD0
#define BMP_REG_RESET         0xE0
#define BMP_REG_STATUS        0xF3
#define BMP_REG_CTRL_MEAS     0xF4
#define BMP_REG_CONFIG        0xF5
#define BMP_REG_PRESS_MSB     0xF7
#define BMP_REG_PRESS_LSB     0xF8
#define BMP_REG_PRESS_XLSB    0xF9
#define BMP_REG_PRESS_MSB     0xF7
#define BMP_REG_TEMP_MSB      0xFA
#define BMP_REG_TEMP_LSB      0xFB
#define BMP_REG_TEMP_XLSB     0xFC


void BMP280_Init();
void BMP280_Write(uint8_t reg, uint8_t data);
uint8_t BMP280_Read(uint8_t reg);
uint32_t BMP280_ADC_T();
uint32_t BMP280_ADC_P();
double BMP280_compensate_T_double(uint32_t adc_T);
double BMP280_compensate_P_double(uint32_t adc_P);

#endif
