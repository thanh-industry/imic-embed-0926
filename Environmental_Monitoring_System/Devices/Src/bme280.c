//Includes
#include "bme280.h"

//Values
static uint8_t id_chip;
static uint8_t calib_buf[24];
static uint8_t bme280_buf[8];

static calib_data calib_vals;
static bme280_adc_data bme280_adc_vals;
static bme280_data bme280_vals;

static BME280_S32_t t_fine;

//Functions

HAL_StatusTypeDef bme280_init(I2C_HandleTypeDef *hi2c)
{
	HAL_StatusTypeDef status;
	uint8_t data;

	//Ghi vào thanh ghi config
	data = 0x00;
	status = HAL_I2C_Mem_Write(hi2c, BME280_I2C_ADDRESS, BME280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	//Ghi vào thanh ghi ctrl_hum
	data = 0x01;
	status = HAL_I2C_Mem_Write(hi2c, BME280_I2C_ADDRESS, BME280_REG_CTRL_HUM, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	//Ghi vào thanh ghi ctrl_meas
	data = 0x25;
	status = HAL_I2C_Mem_Write(hi2c, BME280_I2C_ADDRESS, BME280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	status = bme280_get_triming_param(hi2c, calib_buf);
	if(status != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef bme280_read_id(I2C_HandleTypeDef *hi2c, uint8_t *id_chip)
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Read(hi2c, BME280_I2C_ADDRESS, BME280_REG_ID, I2C_MEMADD_SIZE_8BIT, id_chip, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef bme280_get_triming_param(I2C_HandleTypeDef *hi2c, uint8_t *calib_buf)
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Read(hi2c, BME280_I2C_ADDRESS, 0x88, I2C_MEMADD_SIZE_8BIT, calib_buf, 24, 100);
	if(status != HAL_OK)
	{
		return status;
	}
	calib_vals.dig_T1 = (uint16_t) (calib_buf[1] << 8  | calib_buf[0]);
	calib_vals.dig_T2 = (int16_t)  (calib_buf[3] << 8  | calib_buf[2]);
	calib_vals.dig_T3 = (int16_t)  (calib_buf[5] << 8  | calib_buf[4]);
	calib_vals.dig_P1 = (uint16_t) (calib_buf[7] << 8  | calib_buf[6]);
	calib_vals.dig_P2 = (int16_t)  (calib_buf[9] << 8  | calib_buf[8]);
	calib_vals.dig_P3 = (int16_t)  (calib_buf[11] << 8 | calib_buf[10]);
	calib_vals.dig_P4 = (int16_t)  (calib_buf[13] << 8 | calib_buf[12]);
	calib_vals.dig_P5 = (int16_t)  (calib_buf[15] << 8 | calib_buf[14]);
	calib_vals.dig_P6 = (int16_t)  (calib_buf[17] << 8 | calib_buf[16]);
	calib_vals.dig_P7 = (int16_t)  (calib_buf[19] << 8 | calib_buf[18]);
	calib_vals.dig_P8 = (int16_t)  (calib_buf[21] << 8 | calib_buf[20]);
	calib_vals.dig_P9 = (int16_t)  (calib_buf[23] << 8 | calib_buf[22]);

	status = HAL_I2C_Mem_Read(hi2c, BME280_I2C_ADDRESS, 0xA1, I2C_MEMADD_SIZE_8BIT, calib_buf, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}
	calib_vals.dig_H1 = (uint8_t)  calib_buf[0];

	status = HAL_I2C_Mem_Read(hi2c, BME280_I2C_ADDRESS, 0xE1, I2C_MEMADD_SIZE_8BIT, calib_buf, 7, 100);
	if(status != HAL_OK)
	{
		return status;
	}
	calib_vals.dig_H2 = (int16_t)  (calib_buf[1] << 8  | calib_buf[0]);
	calib_vals.dig_H3 = (uint8_t)  (calib_buf[2]);

	calib_vals.dig_H4 = (int16_t)  (((int8_t)calib_buf[3] * 16) | (calib_buf[4] & 0x0F)); //*16 = << 4
	calib_vals.dig_H5 = (int16_t)  (((int8_t)calib_buf[5] * 16) | (calib_buf[4] >> 4));
	calib_vals.dig_H6 = (int8_t)   (calib_buf[6]);

	return HAL_OK;
}

HAL_StatusTypeDef bme280_get_adc_value(I2C_HandleTypeDef *hi2c, uint8_t *bme280_buf)
{
	HAL_StatusTypeDef status;

	status = HAL_I2C_Mem_Read(hi2c, BME280_I2C_ADDRESS, BME280_REG_PRESS_MSB, I2C_MEMADD_SIZE_8BIT, bme280_buf, 8, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	bme280_adc_vals.pressure = 		(int32_t) (((uint32_t)bme280_buf[0] << 12) | ((uint32_t)bme280_buf[1] << 4) | (bme280_buf[2] >> 4));
	bme280_adc_vals.temperature = 	(int32_t) (((uint32_t)bme280_buf[3] << 12) | ((uint32_t)bme280_buf[4] << 4) | (bme280_buf[5] >> 4));
	bme280_adc_vals.humidity = 		(int32_t) (((uint32_t)bme280_buf[6] << 8)  |  bme280_buf[7]);

	return HAL_OK;
}

//Bosch Datasheet cung cấp ------------------------------------------------------------------------------------------------------------------------------------------------

static BME280_S32_t bme280_compensate_temperature(BME280_S32_t adc_T)
{
    BME280_S32_t var1, var2, T;

    var1 = ((((adc_T >> 3) - ((BME280_S32_t)calib_vals.dig_T1 << 1))) * ((BME280_S32_t)calib_vals.dig_T2)) >> 11;
    var2 = (((((adc_T >> 4) - ((BME280_S32_t)calib_vals.dig_T1)) * ((adc_T >> 4) - ((BME280_S32_t)calib_vals.dig_T1))) >> 12) * ((BME280_S32_t)calib_vals.dig_T3)) >> 14;

    t_fine = var1 + var2;
    T = (t_fine * 5 + 128) >> 8;

    return T; // Trả về nhiệt độ x100 (VD: 2550 = 25.50 DegC)
}

static BME280_U32_t bme280_compensate_pressure(BME280_S32_t adc_P)
{
    BME280_S64_t var1, var2, p;

    var1 = ((BME280_S64_t)t_fine) - 128000;
    var2 = var1 * var1 * (BME280_S64_t)calib_vals.dig_P6;
    var2 = var2 + ((var1 * (BME280_S64_t)calib_vals.dig_P5) << 17);
    var2 = var2 + (((BME280_S64_t)calib_vals.dig_P4) << 35);
    var1 = ((var1 * var1 * (BME280_S64_t)calib_vals.dig_P3) >> 8) + ((var1 * (BME280_S64_t)calib_vals.dig_P2) << 12);
    var1 = (((((BME280_S64_t)1) << 47) + var1)) * ((BME280_S64_t)calib_vals.dig_P1) >> 33;

    if (var1 == 0)
    {
        return 0; // Tránh lỗi chia cho 0
    }

    p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;
    var1 = (((BME280_S64_t)calib_vals.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((BME280_S64_t)calib_vals.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((BME280_S64_t)calib_vals.dig_P7) << 4);

    return (BME280_U32_t)p; // Đơn vị Pa dạng Q24.8 (muốn ra Pa thực: p / 256.0f)
}

static BME280_U32_t bme280_compensate_humidity(BME280_S32_t adc_H)
{
    BME280_S32_t v_x1_u32r;

    v_x1_u32r = (t_fine - ((BME280_S32_t)76800));
    v_x1_u32r = (((((adc_H << 14) - (((BME280_S32_t)calib_vals.dig_H4) << 20) - (((BME280_S32_t)calib_vals.dig_H5) * v_x1_u32r)) +
                   ((BME280_S32_t)16384)) >> 15) * (((((((v_x1_u32r * ((BME280_S32_t)calib_vals.dig_H6)) >> 10) *
                   (((v_x1_u32r * ((BME280_S32_t)calib_vals.dig_H3)) >> 11) + ((BME280_S32_t)32768))) >> 10) +
                   ((BME280_S32_t)2097152)) * ((BME280_S32_t)calib_vals.dig_H2) + 8192) >> 14));

    v_x1_u32r = (v_x1_u32r - (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7) * ((BME280_S32_t)calib_vals.dig_H1)) >> 4));
    v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
    v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);

    return (BME280_U32_t)(v_x1_u32r >> 12); // Đơn vị %RH dạng Q22.10 (muốn ra % thực: v_x1_u32r / 1024.0f)
}

//-----------------------------------------------------------------------------------------------------------------------------------------------------------------------

HAL_StatusTypeDef sensor_get_vals(I2C_HandleTypeDef *hi2c, bme280_data *sensor_data)
{
	HAL_StatusTypeDef status;

	if (sensor_data == NULL)
	{
		return HAL_ERROR;
	}

	status = bme280_get_adc_value(hi2c, bme280_buf);
	if (status != HAL_OK)
	{
		return status;
	}

	int32_t  comp_temp  = bme280_compensate_temperature(bme280_adc_vals.temperature); //Bắt buộc phải chạy trước để lấy biến t_fine, vì 2 hàm dưới cần sử dụng
	uint32_t comp_press = bme280_compensate_pressure(bme280_adc_vals.pressure);
	uint32_t comp_hum   = bme280_compensate_humidity(bme280_adc_vals.humidity);

	sensor_data->temperature = (float)comp_temp / 100.0f;           // Đơn vị: °C
	sensor_data->pressure    = (float)comp_press / 256.0f / 100.0f; // Đơn vị: hPa (Pa / 100)
	sensor_data->humidity    = (float)comp_hum / 1024.0f;           // Đơn vị: %RH

	bme280_vals = *sensor_data;

	return HAL_OK;
}

HAL_StatusTypeDef bme280_trigger_measurement(I2C_HandleTypeDef *hi2c)
{
	HAL_StatusTypeDef status;
	uint8_t data;

	//Ghi vào thanh ghi ctrl_meas
	data = 0x25;
	status = HAL_I2C_Mem_Write(hi2c, BME280_I2C_ADDRESS, BME280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &data, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}
