#ifndef APP_MAIN_H
#define APP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "cmsis_os2.h"
#include <stdint.h>

typedef struct {
    float   temperature;  // Nhiệt độ từ BME280 (VD: 28.2)
    float   humidity;     // Độ ẩm từ BME280
    float   pressure;     // Áp suất từ BME280
    float   voltage;      // Điện áp pin từ ADC (VD: 3.25)

    uint8_t bme280_ok;     // 1: Cảm biến BME280 (I2C1) OK, 0: Lỗi
	uint8_t oled_ok;       // 1: Màn hình OLED (I2C2) OK, 0: Lỗi
	uint8_t flash_ok;      // 1: Flash W25Q32 (SPI1) OK, 0: Lỗi
	uint8_t adc_ok;        // 1: ADC đọc tốt, 0: Lỗi
}	System_Data_t;

extern System_Data_t current_sys_data;

//1. Khai báo Handle Mutex
extern osMutexId_t oledMutexHandle;
extern volatile uint8_t oled_hold_seconds;

//2. Khai báo Handle các Queue dùng chung
extern osMessageQueueId_t uartQueueHandle;
extern osMessageQueueId_t logQueueHandle;

//3. Hàm khởi chạy hệ thống
void app_init(void);

#ifdef __cplusplus
}
#endif

#endif /* APP_MAIN_H */
