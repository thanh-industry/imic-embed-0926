/* logging.h */
#ifndef LOGGING_H
#define LOGGING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdio.h>

typedef enum {
    LOG_LEVEL_DEBUG = 0,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR
} LogLevel_t;

#define CURRENT_LOG_LEVEL LOG_LEVEL_DEBUG

void Log_Init(UART_HandleTypeDef *huart);
void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...);

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

#ifdef __cplusplus
}
#endif

#endif /* LOGGING_H */
