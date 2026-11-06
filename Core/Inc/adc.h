/**
  ******************************************************************************
  * @file    adc.h
  * @brief   Driver ADC cho STM32F407 — Đọc 1 kênh analog + lọc nhiễu
  * @note    Sử dụng: gọi ADC_Init_Module(&hadc1) trước, sau đó dùng
  *          ADC_Read_Raw() / ADC_Read_Voltage() / ADC_Read_Filtered_Voltage()
  ******************************************************************************
  */
#ifndef INC_ADC_H_
#define INC_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

/* ===== Cấu hình module ===== */
#define ADC_VREF              3.3f     // Điện áp tham chiếu chuẩn (Volt)
#define ADC_MAX_VAL           4095.0f  // Thang đo tối đa ADC 12-bit (2^12 - 1)
#define ADC_TIMEOUT_MS        10       // Timeout mỗi lần đọc (ms)
#define ADC_DEFAULT_SAMPLES   10       // Số mẫu lọc mặc định

/* ===== API công khai ===== */

/**
  * @brief  Khởi tạo module ADC
  * @param  hadc: Con trỏ handle ADC do CubeMX sinh (VD: &hadc1)
  * @note   Phải gọi trước khi dùng các hàm khác
  */
void ADC_Init_Module(ADC_HandleTypeDef *hadc);

/**
  * @brief  Đọc giá trị thô (Raw Value) từ ADC
  * @retval Giá trị 12-bit (0 - 4095). Trả 0 nếu chưa init hoặc timeout
  */
uint16_t ADC_Read_Raw(void);

/**
  * @brief  Đọc và quy đổi sang điện áp (Volt)
  * @retval Điện áp đo được (0.0V - 3.3V)
  */
float ADC_Read_Voltage(void);

/**
  * @brief  Đọc điện áp có lọc nhiễu (trung bình cộng nhiều mẫu)
  * @param  samples Số lần lấy mẫu (khuyến nghị 8-16). Nếu = 0 sẽ gán = 1
  * @retval Điện áp trung bình (Volt)
  * @note   Hàm có thể block tới samples × ADC_TIMEOUT_MS (nếu ADC lỗi)
  */
float ADC_Read_Filtered_Voltage(uint8_t samples);

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_H_ */
