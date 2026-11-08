#include "task_sensor.h"
#include "app_main.h"
#include "bme280.h"
#include "adc_sensor.h"
#include <stdint.h>

extern I2C_HandleTypeDef hi2c1;
extern ADC_HandleTypeDef hadc1;

void StartBmeTask(void *argument)
{
	bme280_data bme_sample;
	HAL_StatusTypeDef status;

	status = bme280_init(&hi2c1);
	if(status == HAL_OK)
	{
		current_sys_data.bme280_ok = 1;
	}
	else current_sys_data.bme280_ok = 0;

    for (;;)
    {
    	status = sensor_get_vals(&hi2c1, &bme_sample);
    	if(status == HAL_OK)
    	{
    		current_sys_data.temperature 	= bme_sample.temperature;
    		current_sys_data.humidity		= bme_sample.humidity;
    		current_sys_data.pressure		= bme_sample.pressure;
    		current_sys_data.bme280_ok		= 1;

    		osMessageQueuePut(logQueueHandle, &current_sys_data, 0U, 10U);
    	}
    	else
    	{
    		current_sys_data.bme280_ok 		= 0;
    	}

        osDelay(BME_SAMPLE_PERIOD_MS);
    }
}

void StartVoltTask(void *argument)
{
	float v_val = 0.0f;

	adc_sensor_init(&hadc1);
	current_sys_data.adc_ok = 1;

    for (;;)
    {
    	v_val = adc_sensor_read_filtered_voltage(16);
    	current_sys_data.voltage = v_val;
    	current_sys_data.adc_ok = 1;

        osDelay(VOLT_SAMPLE_PERIOD_MS);
    }
}
