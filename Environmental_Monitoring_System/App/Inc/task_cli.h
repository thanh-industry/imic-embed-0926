#ifndef TASK_CLI_H
#define TASK_CLI_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

/**
 * @brief  Hàm thực thi Task CLI xử lý giao diện dòng lệnh UART
 * @param  argument: Con trỏ tham số (mặc định NULL)
 */
void StartCliTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* TASK_CLI_H */
