/*
 * app_adc.c
 *
 *  Created on: Oct 8, 2026
 *      Author: HACOM VINH
 */
#include "app_adc.h"
#include "app_log.h"
#include "app_system.h"
#include "adc.h"
#include "cmsis_os.h"

#define ADC_VREF_MV  3300
#define ADC_MAX      4095
#define N_SAMPLES    64

extern osMutexId_t SystemDataMutexHandle;

static int ADC_ReadRaw(uint16_t *raw)
{
    HAL_ADC_Start(&hadc);
    if (HAL_ADC_PollForConversion(&hadc, 10) != HAL_OK) {
        HAL_ADC_Stop(&hadc);
        return -1;
    }
    *raw = (uint16_t)HAL_ADC_GetValue(&hadc);
    HAL_ADC_Stop(&hadc);
    return 0;
}

void APP_ADC_TaskLoop(void)
{
    uint16_t raw, vmin, vmax;
    uint32_t sum;
    int n;

    HAL_ADCEx_Calibration_Start(&hadc, ADC_SINGLE_ENDED);

    for (;;) {
        sum = 0; vmin = 0xFFFF; vmax = 0; n = 0;

        for (int i = 0; i < N_SAMPLES; i++) {
            if (ADC_ReadRaw(&raw) == 0) {
                sum += raw;
                if (raw < vmin) vmin = raw;
                if (raw > vmax) vmax = raw;
                n++;
            }
            osDelay(1);
        }

        if (n > 0) {
            int32_t avg_mv = (int32_t)((sum / n) * ADC_VREF_MV / ADC_MAX);
            int32_t pp_mv  = (int32_t)(vmax - vmin) * ADC_VREF_MV / ADC_MAX;

            if (osMutexAcquire(SystemDataMutexHandle, 50) == osOK) {
                g_sys.adc_mv      = avg_mv;
                g_sys.sound_pp_mv = pp_mv;
                osMutexRelease(SystemDataMutexHandle);
            }
            LOG_Post(LOG_ADC_VOLT, avg_mv, 0);
            LOG_Post(LOG_ADC_SOUND, pp_mv, 0);
        } else {
            LOG_Post(LOG_ERROR, 0, "ADC read timeout");
        }
        osDelay(900);
    }
}

