#include "adc.h"
#include "logging.h"   // ← thêm để log lỗi / giá trị
//KHAI BÁO CON TRỎ ADC
static ADC_HandleTypeDef *s_Adc = NULL;

/**
  * @brief  Khởi tạo module ADC dùng cho việc đọc giá trị analog
  * @param  hadc: Con trỏ cấu hình ADC (VD: &hadc1)
  */
void ADC_Init_Module(ADC_HandleTypeDef *hadc)
{
    s_Adc = hadc;

}

/**
  * @brief  Đọc giá trị thô (Raw Value) từ ADC
  * @retval Giá trị raw 12-bit (0..4095), hoặc 0 nếu chưa khởi tạo / lỗi
  */
uint16_t ADC_Read_Raw(void)
{
    if (s_Adc == NULL)
    {
        LOG_E("ADC chua duoc khoi tao!");
        return 0; // Chưa khởi tạo ADC thì thoát, trả về 0 để tránh sập chip
    }

    uint16_t raw_val = 0; // biến chứa giá trị thô đọc được

    // 1. Bắt đầu chuyển đổi
    if (HAL_ADC_Start(s_Adc) != HAL_OK)
    {
        LOG_E("ADC Start that bai!");
        return 0;
    }

    // 2. Chờ chuyển đổi xong (tối đa 10ms)
    if (HAL_ADC_PollForConversion(s_Adc, 10) != HAL_OK)// hỏi liên tục
    {
        LOG_E("ADC Poll timeout!");
        HAL_ADC_Stop(s_Adc);
        return 0;
    }

    // 3. Đọc giá trị thô từ thanh ghi dữ liệu
    raw_val = (uint16_t)HAL_ADC_GetValue(s_Adc);

    // 4. Dừng ADC để giải phóng phần cứng
    HAL_ADC_Stop(s_Adc);

    return raw_val;
}

/**
  * @brief  Đọc và chuyển đổi trực tiếp sang điện áp thực tế
  * @retval Điện áp đo được (đơn vị: V)
  */
float ADC_Read_Voltage(void)
{
    uint16_t raw = ADC_Read_Raw(); // lấy giá trị thô trước
    return ((float)raw * ADC_VREF) / ADC_MAX_VAL; // quy đổi sang điện áp
}

/**
  * @brief  Đọc lấy mẫu trung bình nhiều lần để lọc nhiễu
  * @param  samples: Số lần lấy mẫu (nếu = 0 sẽ tự động gán = 1)
  * @retval Điện áp trung bình sau khi lọc (đơn vị: V)
  */
float ADC_Read_Filtered_Voltage(uint8_t samples)
{
    if (samples == 0)
    {
        samples = 1; // tránh chia cho 0
    }

    uint32_t sum_raw = 0; // tổng các giá trị raw thu được

    for (uint8_t i = 0; i < samples; i++)
    {
        sum_raw += ADC_Read_Raw(); // cộng dồn từng mẫu
    }

    float avg_raw = (float)sum_raw / (float)samples; // tính trung bình
    return (avg_raw * ADC_VREF) / ADC_MAX_VAL;       // quy đổi sang điện áp
}
