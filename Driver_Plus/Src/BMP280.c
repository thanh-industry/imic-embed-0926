#include "BMP280.h"
#include "stdint.h"
typedef struct
{
	uint16_t dig_T1;
	int16_t  dig_T2;
	int16_t  dig_T3;
	uint16_t dig_P1;
	int16_t  dig_P2;
	int16_t  dig_P3;
	int16_t  dig_P4;
	int16_t  dig_P5;
	int16_t  dig_P6;
	int16_t  dig_P7;
	int16_t  dig_P8;
	int16_t  dig_P9;
}BMP280_Calib_t;

void BMP280_Write(uint8_t reg, uint8_t data)
{
	uint8_t tx[2];
	tx[0] = reg & 0x7F;
	tx[1] = data;
	HAL_GPIO_WritePin(PORT_CS, CS, LOW);
	HAL_SPI_Transmit(&hspi1, tx, 2, 100);
	HAL_GPIO_WritePin(PORT_CS, CS, HIGH);
}

uint8_t BMP280_Read(uint8_t reg)
{
	uint8_t tx = reg | 0x80;
	uint8_t rx;
	HAL_GPIO_WritePin(PORT_CS, CS, LOW);
	HAL_SPI_Transmit(&hspi1, &tx, 1, 100);
	HAL_SPI_Receive(&hspi1, &rx, 1, 100);
	HAL_GPIO_WritePin(PORT_CS, CS, HIGH);

	return rx;
}
BMP280_Calib_t data;
void BMP280_Init()
{

	// reset cam bien BMP280
	BMP280_Write(BMP_REG_RESET, 0xB6);

	osDelay(100);
	// cho qua trinh sao chep chac thanh ghi bu 0xA1 - 0x88
	while((BMP280_Read(BMP_REG_STATUS) & 1) == 1);
	// lay 24 byte he so hieu chuan va ghep vao  T1,T2,T3,...,P9.
	data.dig_T1 = (uint16_t)BMP280_Read(0x88) | (BMP280_Read(0x89) << 8);
	data.dig_T2 = (int16_t)BMP280_Read(0x8A)  | (BMP280_Read(0x8B) << 8);
	data.dig_T3 = (int16_t)BMP280_Read(0x8C)  | (BMP280_Read(0x8D) << 8);
	data.dig_P1 = (uint16_t)BMP280_Read(0x8E) | (BMP280_Read(0x8F) << 8);
	data.dig_P2 = (int16_t)BMP280_Read(0x90)  | (BMP280_Read(0x91) << 8);
	data.dig_P3 = (int16_t)BMP280_Read(0x92)  | (BMP280_Read(0x93) << 8);
	data.dig_P4 = (int16_t)BMP280_Read(0x94)  | (BMP280_Read(0x95) << 8);
	data.dig_P5 = (int16_t)BMP280_Read(0x96)  | (BMP280_Read(0x97) << 8);
	data.dig_P6 = (int16_t)BMP280_Read(0x98)  | (BMP280_Read(0x99) << 8);
	data.dig_P7 = (int16_t)BMP280_Read(0x9A)  | (BMP280_Read(0x9B) << 8);
	data.dig_P8 = (int16_t)BMP280_Read(0x9C)  | (BMP280_Read(0x9D) << 8);
	data.dig_P9 = (int16_t)BMP280_Read(0x9E)  | (BMP280_Read(0x9F) << 8);

	// config thoi gian stanby cua BMP290
	BMP280_Write(BMP_REG_CONFIG, 0x80);

	//
	BMP280_Write(BMP_REG_CTRL_MEAS, 0x2F);
}

uint32_t BMP280_ADC_T()
{
	uint32_t adc_t = (uint32_t) (BMP280_Read(BMP_REG_TEMP_MSB) << 12)
			       | (uint32_t) (BMP280_Read(BMP_REG_TEMP_LSB) << 4)
				   | (uint32_t) (BMP280_Read(BMP_REG_TEMP_XLSB) >> 4);
	return adc_t;
}
uint32_t BMP280_ADC_P()
{
	uint32_t adc_p = (uint32_t) (BMP280_Read(BMP_REG_PRESS_MSB) << 12)
				   | (uint32_t) (BMP280_Read(BMP_REG_PRESS_LSB) << 4)
				   | (uint32_t) (BMP280_Read(BMP_REG_PRESS_XLSB) >> 4);
	return adc_p;
}
// Returns temperature in DegC, double precision. Output value of “51.23” equals 51.23 DegC.
// t_fine carries fine temperature as global value
int32_t t_fine;
double BMP280_compensate_T_double(uint32_t adc_T)
{
	double var1, var2, T;
	var1 = (((double) adc_T) / 16384.0 - ((double) data.dig_T1)/1024.0) * ((double) data.dig_T2);
	var2 = ((((double) adc_T) / 131072.0 - ((double) data.dig_T1)/8192.0)*
	(((double)adc_T)/131072.0 - ((double) data.dig_T1)/8192.0))
			* ((double) data.dig_T3);
	t_fine = (int32_t)(var1 + var2);
	T = (var1 + var2) / 5120.0;
	return T;
}
// Returns pressure in Pa as double. Output value of “96386.2” equals 96386.2 Pa = 963.862 hPa
double BMP280_compensate_P_double(uint32_t adc_P)
{
	double var1, var2, p;
	var1 = ((double) t_fine / 2.0) - 64000.0;
	var2 = var1 * var1 * ((double) data.dig_P6) / 32768.0;
	var2 = var2 + var1 * ((double) data.dig_P5) * 2.0;
	var2 = (var2 / 4.0) + (((double) data.dig_P4) * 65536.0);
	var1 = (((double) data.dig_P3) * var1 * var1 / 524288.0
			+ ((double) data.dig_P2) * var1) / 524288.0;
	var1 = (1.0 + var1 / 32768.0) * ((double) data.dig_P1);
	if (var1 == 0.0)
	{
		return 0; // avoid exception caused by division by zero
	}
	p = 1048576.0 - (double)adc_P;
	p = (p - (var2 / 4096.0)) * 6250.0 / var1;
	var1 = ((double) data.dig_P9) * p * p / 2147483648.0;
	var2 = p * ((double) data.dig_P8) / 32768.0;
	p = p + (var1 + var2 + ((double) data.dig_P7)) / 16.0;
	return p;
}
