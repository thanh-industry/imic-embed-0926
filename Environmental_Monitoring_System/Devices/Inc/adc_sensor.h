#ifndef ADC_SENSOR_H
#define ADC_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

#define ADC_SENSOR_VREF         3.3f     //Điện áp tham chiếu chuẩn (Volt)
#define ADC_SENSOR_MAX_VAL      4095.0f  //Thang đo tối đa ADC 12-bit (2^12 - 1)

/**
  * @brief  Khởi tạo module ADC Sensor
  * @param  hadc: Con trỏ trỏ đến ADC_HandleTypeDef sinh bởi STM32CubeMX
  */
void adc_sensor_init(ADC_HandleTypeDef *hadc);

/**
  * @brief  Đọc giá trị thô (Raw Value) từ ADC
  * @return Giá trị số nguyên 12-bit (0 - 4095)
  */
uint16_t adc_sensor_read_raw(void);

/**
  * @brief  Đọc và chuyển đổi trực tiếp sang điện áp thực tế
  * @return Điện áp thực tế tính bằng Volt (0.0V - 3.3V)
  */
float adc_sensor_read_voltage(void);

/**
  * @brief  Đọc lấy mẫu trung bình nhiều lần để lọc nhiễu rung tay khi vặn biến trở
  * @param  samples Số lần lấy mẫu (khuyên dùng từ 8 đến 16 lần)
  * @return Điện áp trung bình đã được lọc mượt (Volt)
  */
float adc_sensor_read_filtered_voltage(uint8_t samples);

#ifdef __cplusplus
}
#endif

#endif /* ADC_SENSOR_H */
