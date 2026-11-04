#include "log.h"

#define LOG_BUFFER_SIZE 256

static const char *log_level_str(log_level_t level)
{
    switch (level)
    {
        case LOG_LEVEL_ERROR:   return "E";
        case LOG_LEVEL_WARN:    return "W";
        case LOG_LEVEL_INFO:    return "I";
        case LOG_LEVEL_DEBUG:   return "D";
        case LOG_LEVEL_VERBOSE: return "V";
        default:                return "?";
    }
}

void log_printf(log_level_t level,
                const char *tag,
                const char *file,
                int line,
                const char *fmt, ...)
{
    /* Tránh cảnh báo unused parameter từ trình biên dịch */
    (void)file;
    (void)line;

    char buffer[LOG_BUFFER_SIZE];

    int offset = snprintf(buffer,
                          sizeof(buffer),
                          "[%lu][%s][%s] ",
                          (unsigned long)HAL_GetTick(),
                          log_level_str(level),
                          tag);

    if (offset < 0 || offset >= (int)sizeof(buffer))
    {
        return;
    }

    va_list args;
    va_start(args, fmt);

    int len = vsnprintf(&buffer[offset],
                        sizeof(buffer) - (size_t)offset,
                        fmt,
                        args);

    va_end(args);

    if (len < 0)
    {
        return;
    }

    size_t total_len = strnlen(buffer, sizeof(buffer));

    /* Thêm ký tự xuống dòng CR/LF */
    if (total_len < sizeof(buffer) - 2)
    {
        buffer[total_len++] = '\r';
        buffer[total_len++] = '\n';
    }

    /* Truyền dữ liệu qua cổng USART2 */
    HAL_UART_Transmit(&huart2,
                      (uint8_t *)buffer,
                      (uint16_t)total_len,
                      100);
}
