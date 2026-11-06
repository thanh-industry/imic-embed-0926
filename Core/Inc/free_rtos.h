/* freertos.h */
#ifndef FREERTOS_APP_H
#define FREERTOS_APP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/* ===== Biến toàn cục chia sẻ giữa các Task ===== */
extern float    g_current_voltage;   /* Điện áp ADC đọc được (V) */
extern float    g_temperature;       /* Nhiệt độ cảm biến I2C (°C) */
extern uint8_t  g_i2c_status;        /* 1: OK, 0: ERROR */
extern uint8_t  g_spi_status;        /* 1: OK, 0: ERROR */
extern uint32_t g_system_tick;       /* Số lần ADC task chạy */

/* ===== Khai báo các Task ===== */
void StartADCTask(void *argument);
void StartCommandTask(void *argument);
void StartSensorTask(void *argument);
void StartSPITask(void *argument);

/* ===== Khởi tạo FreeRTOS ===== */
void MX_FREERTOS_Init(void);

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_APP_H */
