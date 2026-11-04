#ifndef LOG_H
#define LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

/* =========================
 * Log Level
 * ========================= */
#define LOG_LEVEL_NONE    0
#define LOG_LEVEL_ERROR   1
#define LOG_LEVEL_WARN    2
#define LOG_LEVEL_INFO    3
#define LOG_LEVEL_DEBUG   4
#define LOG_LEVEL_VERBOSE 5

typedef int log_level_t;

/* =========================
 * Configuration
 * ========================= */
#ifndef LOG_LEVEL
#define LOG_LEVEL LOG_LEVEL_INFO
#endif

/* Khai báo extern huart2 từ main.c */
extern UART_HandleTypeDef huart2;

/* =========================
 * Log Function
 * ========================= */
void log_printf(log_level_t level,
                const char *tag,
                const char *file,
                int line,
                const char *fmt, ...);

/* =========================
 * Log Macros
 * ========================= */
#if LOG_LEVEL >= LOG_LEVEL_ERROR
#define LOGE(tag, fmt, ...) \
    log_printf(LOG_LEVEL_ERROR, tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOGE(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_WARN
#define LOGW(tag, fmt, ...) \
    log_printf(LOG_LEVEL_WARN, tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOGW(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_INFO
#define LOGI(tag, fmt, ...) \
    log_printf(LOG_LEVEL_INFO, tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOGI(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_DEBUG
#define LOGD(tag, fmt, ...) \
    log_printf(LOG_LEVEL_DEBUG, tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOGD(tag, fmt, ...)
#endif

#if LOG_LEVEL >= LOG_LEVEL_VERBOSE
#define LOGV(tag, fmt, ...) \
    log_printf(LOG_LEVEL_VERBOSE, tag, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#else
#define LOGV(tag, fmt, ...)
#endif

#ifdef __cplusplus
}
#endif

#endif /* LOG_H */
