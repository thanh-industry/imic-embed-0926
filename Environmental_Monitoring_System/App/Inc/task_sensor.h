#ifndef TASK_SENSOR_H
#define TASK_SENSOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

#define BME_SAMPLE_PERIOD_MS 1000U
#define VOLT_SAMPLE_PERIOD_MS 1000U

/**
 * @brief  Hàm thực thi Task Bme lấy dữ liệu từ môi trường
 * @param  argument: Con trỏ tham số (mặc định NULL)
 */
void StartBmeTask(void *argument);

/**
 * @brief  Hàm thực thi Task Volt đo điện áp rơi trên biến trở
 * @param  argument: Con trỏ tham số (mặc định NULL)
 */
void StartVoltTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* TASK_SENSOR_H */
