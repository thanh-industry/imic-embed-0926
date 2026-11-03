#include "logging.h"
#include <stdarg.h>// duyệt tham số để truyền vào hàm Log_Print
#include <string.h>
#include <time.h>

static UART_HandleTypeDef *s_LogUart = NULL;
// khởi tạo 1 biến con trỏ UART dạng static đóng gói trông file đang code

/**
  * @brief  Khởi tạo giao tiếp UART dùng cho việc in log
  * @param  huart: Con trỏ cấu hình UART (VD: &huart1)
  */
void Log_Init(UART_HandleTypeDef *huart) {
    s_LogUart = huart;
}

/**
  * @brief  Hàm xử lý định dạng chuỗi và đẩy dữ liệu ra UART
  */
void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...) {
    if (s_LogUart == NULL) {
        return; // Chưa khởi tạo UART thì thoát
    }

    char header_buf[128];// tạo mảng chứa tiêu đề
    char msg_buf[256];// tạo mảng chứa nội dung
    int header_len = 0;// đếm tiêu đề bao nhiêu kí tự
    int msg_len = 0;// đém kí tự nội dung

    // Gắn nhãn phân loại mức độ log
    const char *level_str = "";
    switch (level) {
        case LOG_LEVEL_DEBUG: level_str = "DBG";
        break;
        case LOG_LEVEL_INFO:  level_str = "INF";
        break;
        case LOG_LEVEL_WARN:  level_str = "WRN";
        break;
        case LOG_LEVEL_ERROR: level_str = "ERR";
        break;
    }

    // Lấy tên file ngắn gọn từ đường dẫn tuyệt đối
    const char *filename = strrchr(file, '/');
    if (filename == NULL) {
        filename = strrchr(file, '\\');
    }
    filename = (filename != NULL) ? (filename + 1) : file;

    // Tạo phần tiêu đề log kèm thời gian hệ thống (Tick), tên file và dòng code
    header_len = snprintf(header_buf, sizeof(header_buf), "[%lu][%s][%s:%d] ",
                          HAL_GetTick(), level_str, filename, line);

    if (header_len > 0) {
        HAL_UART_Transmit(s_LogUart, (uint8_t*)header_buf, header_len, HAL_MAX_DELAY);
    }

    // Định dạng nội dung thông điệp cần log truyền vào
    va_list args;
    va_start(args, format);
    msg_len = vsnprintf(msg_buf, sizeof(msg_buf), format, args);
    va_end(args);

    if (msg_len > 0) {
        HAL_UART_Transmit(s_LogUart, (uint8_t*)msg_buf, msg_len, HAL_MAX_DELAY);
    }

    // Ký tự xuống dòng kết thúc bản tin log
    HAL_UART_Transmit(s_LogUart, (uint8_t*)"\r\n", 2, HAL_MAX_DELAY);
}
