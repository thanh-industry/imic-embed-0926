#ifndef INC_ADC_H_
#define INC_ADC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "main.h"

/* ===== Cấu hình module ===== */
#define ADC_VREF              3.3f
#define ADC_MAX_VAL           4095.0f
#define ADC_TIMEOUT_MS        10U
#define ADC_DEFAULT_SAMPLES   10U
#define ADC_ERROR_RAW_VAL     0xFFFFU

/* ===== API công khai ===== */
void     ADC_Init_Module(ADC_HandleTypeDef *hadc);
uint16_t ADC_Read_Raw(void);
float    ADC_Read_Voltage(void);
float    ADC_Read_Filtered_Voltage(uint8_t samples);

#ifdef __cplusplus
}
#endif

#endif /* INC_ADC_H_ */
