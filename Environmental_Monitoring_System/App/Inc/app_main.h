#ifndef APP_MAIN_H
#define APP_MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "cmsis_os2.h"
#include "cli_parser.h"

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
