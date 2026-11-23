/* logging.c */
#include "logging.h"
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "cmsis_os.h"

#define LOG_BUF_SIZE           256U
#define LOG_MUTEX_TIMEOUT_MS   50U
#define LOG_UART_TIMEOUT_MS    20U

static UART_HandleTypeDef *s_LogUart = NULL;
static uint8_t             s_rx_byte_temp = 0U;

/* Khai bao Mutex UART va ham day ISR tu freertos.c */
extern osMutexId_t uartMutexHandle;
extern void        UART_RxByteFromISR(uint8_t rx_byte);

void Log_Init(UART_HandleTypeDef *huart) {
    s_LogUart = huart;
    if (s_LogUart != NULL) {
        (void)HAL_UART_Receive_IT(s_LogUart, &s_rx_byte_temp, 1U);
    }
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
    if ((s_LogUart != NULL) && (huart->Instance == s_LogUart->Instance)) {
        UART_RxByteFromISR(s_rx_byte_temp);
        (void)HAL_UART_Receive_IT(s_LogUart, &s_rx_byte_temp, 1U);
    }
}

/* Khoi phuc ngat nhan UART neu xay ra loi Overrun / Framing / Noise */
void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart) {
    if ((s_LogUart != NULL) && (huart->Instance == s_LogUart->Instance)) {
        __HAL_UART_CLEAR_OREFLAG(huart);
        __HAL_UART_CLEAR_NEFLAG(huart);
        __HAL_UART_CLEAR_FEFLAG(huart);
        (void)HAL_UART_Receive_IT(s_LogUart, &s_rx_byte_temp, 1U);
    }
}

void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...) {
    char        log_buf[LOG_BUF_SIZE];
    const char *level_str;
    const char *filename;
    const char *slash_ptr;
    int         prefix_len;
    int         msg_len;
    uint16_t    total_len;
    va_list     args;

    if ((s_LogUart == NULL) || (format == NULL)) {
        return;
    }

    switch (level) {
        case LOG_LEVEL_DEBUG: level_str = "DBG"; break;
        case LOG_LEVEL_INFO:  level_str = "INF"; break;
        case LOG_LEVEL_WARN:  level_str = "WRN"; break;
        case LOG_LEVEL_ERROR: level_str = "ERR"; break;
        default:              level_str = "LOG"; break;
    }

    filename = (file != NULL) ? file : "unknown";
    slash_ptr = strrchr(filename, '/');
    if (slash_ptr == NULL) {
        slash_ptr = strrchr(filename, '\\');
    }
    if (slash_ptr != NULL) {
        filename = slash_ptr + 1;
    }

    prefix_len = snprintf(log_buf, sizeof(log_buf), "[%lu][%s][%s:%d] ",
                          (unsigned long)HAL_GetTick(), level_str, filename, line);
    if (prefix_len < 0) {
        return;
    }

    if ((size_t)prefix_len < (sizeof(log_buf) - 3U)) {
        va_start(args, format);
        msg_len = vsnprintf(&log_buf[prefix_len],
                            sizeof(log_buf) - (size_t)prefix_len - 2U,
                            format, args);
        va_end(args);

        if (msg_len < 0) {
            msg_len = 0;
        }
        total_len = (uint16_t)prefix_len + (uint16_t)msg_len;
        if (total_len > (uint16_t)(sizeof(log_buf) - 3U)) {
            total_len = (uint16_t)(sizeof(log_buf) - 3U);
        }
    } else {
        total_len = (uint16_t)(sizeof(log_buf) - 3U);
    }

    log_buf[total_len++] = '\r';
    log_buf[total_len++] = '\n';
    log_buf[total_len]   = '\0';

    /* Bao ve UART bang Mutex co timeout, tuyet doi khong dung osWaitForever */
    if (uartMutexHandle != NULL) {
        if (osMutexAcquire(uartMutexHandle, pdMS_TO_TICKS(LOG_MUTEX_TIMEOUT_MS)) == osOK) {
            (void)HAL_UART_Transmit(s_LogUart, (uint8_t *)log_buf, total_len, LOG_UART_TIMEOUT_MS);
            configASSERT(osMutexRelease(uartMutexHandle) == osOK);
        }
    } else {
        /* Cho phep in log truoc khi RTOS Kernel khoi chay */
        (void)HAL_UART_Transmit(s_LogUart, (uint8_t *)log_buf, total_len, LOG_UART_TIMEOUT_MS);
    }
}
///
