/* logging.c */
#include "logging.h"
#include <stdarg.h>
#include <string.h>
#include "cmsis_os.h" // Cần thêm để dùng Queue

static UART_HandleTypeDef *s_LogUart = NULL;

/* Khai báo Mutex UART từ freertos.c để dùng chung */
extern osMutexId_t uartMutexHandle;

/* Khai báo Queue từ freertos.c để nhận lệnh */
extern osMessageQueueId_t uartQueueHandle;

/* Biến tạm để hứng byte từ Ngắt */
static uint8_t rx_byte_temp;

/**
  * @brief  Khởi tạo giao tiếp UART dùng cho việc in log và nhận lệnh
  * @param  huart: Con trỏ cấu hình UART (VD: &huart1)
  */
void Log_Init(UART_HandleTypeDef *huart) {
    s_LogUart = huart;
    // Bắt đầu nhận ngắt 1 byte
    HAL_UART_Receive_IT(s_LogUart, &rx_byte_temp, 1);
}

/**
  * @brief  Hàm Callback tự động gọi khi nhận xong 1 byte qua Ngắt UART
  *         (Đã chuyển sang dùng Queue)
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) // Đảm bảo đúng cổng UART đang dùng
    {
        // Đẩy byte nhận được vào Queue (Tham số cuối = 0 vì chạy trong ISR)
        osMessageQueuePut(uartQueueHandle, &rx_byte_temp, 0, 0);

        // Tiếp tục nhận byte tiếp theo
        HAL_UART_Receive_IT(huart, &rx_byte_temp, 1);
    }
}

/**
  * @brief  Hàm xử lý định dạng chuỗi và đẩy dữ liệu ra UART
  */
void Log_Print(LogLevel_t level, const char *file, int line, const char *format, ...) {
    if (s_LogUart == NULL) return;

    char header_buf[128];
    char msg_buf[256];
    int header_len = 0;
    int msg_len = 0;

    const char *level_str = "";
    switch (level) {
        case LOG_LEVEL_DEBUG: level_str = "DBG"; break;
        case LOG_LEVEL_INFO:  level_str = "INF"; break;
        case LOG_LEVEL_WARN:  level_str = "WRN"; break;
        case LOG_LEVEL_ERROR: level_str = "ERR"; break;
    }

    // Lấy tên file ngắn gọn (bỏ đường dẫn)
    const char *filename = strrchr(file, '/');
    if (filename == NULL) filename = strrchr(file, '\\');
    filename = (filename != NULL) ? (filename + 1) : file;

    // Định dạng phần tiêu đề (Header)
    header_len = snprintf(header_buf, sizeof(header_buf), "[%lu][%s][%s:%d] ",
                          HAL_GetTick(), level_str, filename, line);

    // Định dạng phần nội dung (Message)
    va_list args;
    va_start(args, format);
    msg_len = vsnprintf(msg_buf, sizeof(msg_buf), format, args);
    va_end(args);

    // === BẢO VỆ UART BẰNG MUTEX (Chống đè chữ khi nhiều Task cùng in) ===
    if (uartMutexHandle != NULL && osMutexAcquire(uartMutexHandle, osWaitForever) == osOK)
    {
        if (header_len > 0) {
            HAL_UART_Transmit(s_LogUart, (uint8_t*)header_buf, header_len, 100);
        }
        if (msg_len > 0) {
            HAL_UART_Transmit(s_LogUart, (uint8_t*)msg_buf, msg_len, 100);
        }
        HAL_UART_Transmit(s_LogUart, (uint8_t*)"\r\n", 2, 100);

        osMutexRelease(uartMutexHandle);
    }
}
