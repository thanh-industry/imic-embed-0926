/* adc.c */
#include "adc.h"
#include "logging.h"

static ADC_HandleTypeDef *s_Adc = NULL;

void ADC_Init_Module(ADC_HandleTypeDef *hadc) {
    s_Adc = hadc;
}

uint16_t ADC_Read_Raw(void) {
    uint16_t raw_val;

    if (s_Adc == NULL) {
        return ADC_ERROR_RAW_VAL;
    }

    if (HAL_ADC_Start(s_Adc) != HAL_OK) {
        return ADC_ERROR_RAW_VAL;
    }

    if (HAL_ADC_PollForConversion(s_Adc, ADC_TIMEOUT_MS) != HAL_OK) {
        (void)HAL_ADC_Stop(s_Adc);
        return ADC_ERROR_RAW_VAL;
    }

    raw_val = (uint16_t)HAL_ADC_GetValue(s_Adc);
    (void)HAL_ADC_Stop(s_Adc);

    return raw_val;
}

float ADC_Read_Voltage(void) {
    uint16_t raw;

    raw = ADC_Read_Raw();
    if (raw == ADC_ERROR_RAW_VAL) {
        LOG_E("[ADC] Loi doc ADC!");
        return 0.0f;
    }

    return ((float)raw * ADC_VREF) / ADC_MAX_VAL;
}

float ADC_Read_Filtered_Voltage(uint8_t samples) {
    uint32_t sum_raw   = 0UL;
    uint8_t  valid_cnt = 0U;
    uint8_t  i;
    uint16_t raw;
    float    avg_raw;

    if (s_Adc == NULL) {
        LOG_E("[ADC] Chua khoi tao module ADC!");
        return 0.0f;
    }

    if (samples == 0U) {
        samples = 1U;
    }

    for (i = 0U; i < samples; i++) {
        raw = ADC_Read_Raw();
        if (raw != ADC_ERROR_RAW_VAL) {
            sum_raw += (uint32_t)raw;
            valid_cnt++;
        }
    }

    if (valid_cnt == 0U) {
        LOG_E("[ADC] Tat ca mau doc ADC deu loi!");
        return 0.0f;
    }

    avg_raw = (float)sum_raw / (float)valid_cnt;
    return (avg_raw * ADC_VREF) / ADC_MAX_VAL;
}
