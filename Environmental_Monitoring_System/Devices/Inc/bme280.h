#ifndef __BME280_H
#define __BME280_H

#ifdef __cplusplus
extern "C" {
#endif

//Includes
#include "stm32f4xx_hal.h"

//Define
#define BME280_I2C_ADDRESS (0x76 << 1)
#define BME280_REG_PRESS_MSB	0xF7
#define BME280_REG_CONFIG 		0xF5
#define BME280_REG_CTRL_MEAS	0xF4
#define BME280_REG_STATUS		0xF3
#define BME280_REG_CTRL_HUM		0xF2
#define BME280_REG_RESET		0xE0
#define BME280_REG_ID 			0xD0

//4.2.3 Note typedef
typedef int32_t BME280_S32_t;
typedef uint32_t BME280_U32_t;
typedef int64_t BME280_S64_t;

//Struct
	typedef struct
	{
		//Calibration of temperature
		uint16_t dig_T1;
		int16_t dig_T2;
		int16_t dig_T3;

		//Calibration of pressure
		uint16_t dig_P1;
		int16_t dig_P2;
		int16_t dig_P3;
		int16_t dig_P4;
		int16_t dig_P5;
		int16_t dig_P6;
		int16_t dig_P7;
		int16_t dig_P8;
		int16_t dig_P9;

		//Calibration of humidity
		uint8_t dig_H1;
		int16_t dig_H2;
		uint8_t dig_H3;
		int16_t dig_H4;
		int16_t dig_H5;
		int8_t dig_H6;
	} calib_data;

	typedef struct
	{
		int32_t temperature;
		int32_t pressure;
		int32_t humidity;
	} bme280_adc_data;

	typedef struct
	{
		float temperature;
		float pressure;
		float humidity;
	} bme280_data;

//Function prototyppes

	/**
		 * @brief Khởi tạo cảm biến (ghi config, ctrl_hum, ctrl_meas)
		 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
		 * @return HAL_StatusTypeDef
		 */
	HAL_StatusTypeDef bme280_init(I2C_HandleTypeDef *hi2c);

	/**
	 * @brief Đọc id chip của bme280
	 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @param id_chip Con trỏ trỏ tới nơi chưa mã id của bme280
	 * @return HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef bme280_read_id(I2C_HandleTypeDef *hi2c, uint8_t *id_chip);

	/**
	 * @brief Đọc thông số hiệu chỉnh (trimming parameters)
	 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @param calib_buf Con trỏ trỏ tới nơi chứa các thông số hiệu chỉnh
	 * @return HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef bme280_get_triming_param(I2C_HandleTypeDef *hi2c, uint8_t *calib_buf);

	/**
	 * @brief Lấy giá trị adc của nhiệt độ & áp suất & độ ẩm
	 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @param bme280_buf Con trỏ trỏ tới nơi chứa các giá trị adc của t,p,h
	 * @return HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef bme280_get_adc_value(I2C_HandleTypeDef *hi2c, uint8_t *bme280_buf);

	/**
	 * @brief Đọc và tính toán giá trị nhiệt độ, áp suất, độ ẩm thực tế
	 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @param calib Con trỏ chứa hệ số calib đã lấy được
	 * @param sensor_data Con trỏ chứa kết quả đo sau khi tính toán
	 * @return HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef sensor_get_vals(I2C_HandleTypeDef *hi2c, bme280_data *sensor_data);

	/**
	 * @brief Kích hoạt lấy mẫu 1 lần
	 * @param hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @return @return HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef bme280_trigger_measurement(I2C_HandleTypeDef *hi2c);


#ifdef __cplusplus
}
#endif

#endif /* __BME280_H*/
