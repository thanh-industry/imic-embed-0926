#ifndef LOGGING_H
#define LOGGING_H

#include "main.h"
#include <stdio.h>

// Định nghĩa các mức độ log (Log Levels)
typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel_t;

// Cấu hình mức độ log hiện tại của hệ thống (có thể thay đổi linh hoạt)
#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG

// Khởi tạo module log (truyền con trỏ UART sử dụng)
void Log_Init(UART_HandleTypeDef *huart);

// Hàm ghi log chính (không nên gọi trực tiếp, hãy dùng macro bên dưới)
void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...);

// Các Macro tiện ích để sử dụng nhanh chóng trong code
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
