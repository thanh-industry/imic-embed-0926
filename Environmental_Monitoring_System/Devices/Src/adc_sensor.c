#include "adc_sensor.h"

/* Con trỏ tĩnh lưu trữ handle phần cứng nội bộ của file */
static ADC_HandleTypeDef *p_hadc = NULL;

void adc_sensor_init(ADC_HandleTypeDef *hadc)
{
    p_hadc = hadc;
}

uint16_t adc_sensor_read_raw(void)
{
    if (p_hadc == NULL)
    {
        return 0;
    }

    uint16_t raw_val = 0;

    /* 1. Kích hoạt ngoại vi ADC bắt đầu chuyển đổi */
    if (HAL_ADC_Start(p_hadc) == HAL_OK)
    {
        /* 2. Chờ chuyển đổi hoàn tất với timeout 10ms */
        if (HAL_ADC_PollForConversion(p_hadc, 10) == HAL_OK)
        {
            /* 3. Đọc dữ liệu từ thanh ghi kết quả */
            raw_val = (uint16_t)HAL_ADC_GetValue(p_hadc);
        }

        /* 4. Tắt ADC sau khi chuyển đổi xong để tiết kiệm năng lượng */
        HAL_ADC_Stop(p_hadc);
    }

    return raw_val;
}

float adc_sensor_read_voltage(void)
{
    uint16_t raw = adc_sensor_read_raw();
    return ((float)raw * ADC_SENSOR_VREF) / ADC_SENSOR_MAX_VAL;
}

float adc_sensor_read_filtered_voltage(uint8_t samples)
{
    if (samples == 0)
    {
        samples = 1;
    }

    uint32_t sum_raw = 0;

    for (uint8_t i = 0; i < samples; i++)
    {
        sum_raw += adc_sensor_read_raw();
    }

    float avg_raw = (float)sum_raw / (float)samples;
    return (avg_raw * ADC_SENSOR_VREF) / ADC_SENSOR_MAX_VAL;
}
