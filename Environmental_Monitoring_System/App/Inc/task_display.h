#ifndef TASK_DISPLAY_H
#define TASK_DISPLAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "cmsis_os2.h"

#define DISPLAY_REFRESH_PERIOD_MS 1000U

/**
 * @brief  Hàm thực thi Task Oled để hiển thị nội dung lên oled
 * @param  argument: Con trỏ tham số (mặc định NULL)
 */
void StartOledTask(void *argument);

#ifdef __cplusplus
}
#endif

#endif /* TASK_DISPLAY_H */
