#ifndef TASK_STORAGE_H
#define TASK_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

#define STORAGE_START_ADDR 0x00010000U //Sector 16, chừa sector đầu tiên cho thông tin cấu hình nếu cần
#define STORAGE_QUEUE_TIMEOUT_MS osWaitForever

/**
 * @brief  Hàm thực thi Task Flash dùng để lưu các giá trị vào chip nhớ W25Q32
 * @param  argument: Con trỏ tham số (mặc định NULL)
 */
void StartFlashTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* TASK_STORAGE_H */
