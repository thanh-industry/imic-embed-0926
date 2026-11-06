/* logging.h */
#ifndef LOGGING_H
#define LOGGING_H

#include "main.h"
#include "cmsis_os.h" // Thêm vào để dùng osThreadId_t
#include <stdio.h>

// Định nghĩa các mức độ log (Log Levels)
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel_t;

// Cấu hình mức độ log hiện tại của hệ thống
#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG

/* Kích thước bộ đệm nhận UART */
#define UART_RX_BUFFER_SIZE 128

// Khởi tạo module log (truyền con trỏ UART sử dụng)
void Log_Init(UART_HandleTypeDef *huart);

// Hàm ghi log chính
void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...);

/* ===== CÁC HÀM MỚI CHO VIỆC NHẬN LỆNH QUA NGẮT UART ===== */
void Log_SetCmdTask(osThreadId_t task_id); // Gán Task ID để ngắt đánh thức
uint16_t Log_Available(void);              // Kiểm tra có bao nhiêu byte trong bộ đệm
uint8_t Log_Read(void);                    // Đọc 1 byte từ bộ đệm

// Các Macro tiện ích
#define LOG_D(fmt, ...) do { if (CURRENT_LOG_LEVEL <= LOG_LEVEL_DEBUG) \
                            Log_Print(LOG_LEVEL_DEBUG, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
                            } while(0)

#define LOG_I(fmt, ...) do { if (CURRENT_LOG_LEVEL <= LOG_LEVEL_INFO) \
                            Log_Print(LOG_LEVEL_INFO, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
                            } while(0)

#define LOG_W(fmt, ...) do { if (CURRENT_LOG_LEVEL <= LOG_LEVEL_WARN) \
                            Log_Print(LOG_LEVEL_WARN, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
                            } while(0)

#define LOG_E(fmt, ...) do { if (CURRENT_LOG_LEVEL <= LOG_LEVEL_ERROR) \
                            Log_Print(LOG_LEVEL_ERROR, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
                            } while(0)

#endif /* LOGGING_H */
