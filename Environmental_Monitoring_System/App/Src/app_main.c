//Include
#include "app_main.h"

// 1. Cấp phát bộ nhớ thực tế cho biến dữ liệu toàn cục
System_Data_t current_sys_data = {0};

// 2. Cấp phát các Handle
osMutexId_t   		oledMutexHandle 	= NULL;
osMessageQueueId_t 	uartQueueHandle 	= NULL;
osMessageQueueId_t 	logQueueHandle 		= NULL;

// 3. Khai báo các hàm Task
extern void StartCliTask(void *argument);
extern void StartBmeTask(void *argument);
extern void StartVoltTask(void *argument);
extern void StartOledTask(void *argument);
extern void StartFlashTask(void *argument);

// 4. Định nghĩa thuộc tính cho các task

osThreadId_t cliTaskHandle;
const osThreadAttr_t cliTask_attributes = {
    .name       = "cliTask",
    .stack_size = 512 * 4,                       // 2048 bytes
    .priority   = (osPriority_t) osPriorityAboveNormal,
};

osThreadId_t bmeTaskHandle;
const osThreadAttr_t bmeTask_attributes = {
    .name       = "bmeTask",
    .stack_size = 256 * 4,                       // 1024 bytes
    .priority   = (osPriority_t) osPriorityNormal,
};

osThreadId_t voltTaskHandle;
const osThreadAttr_t voltTask_attributes = {
    .name       = "voltTask",
    .stack_size = 128 * 4,                       // 512 bytes
    .priority   = (osPriority_t) osPriorityNormal,
};

osThreadId_t oledTaskHandle;
const osThreadAttr_t oledTask_attributes = {
    .name       = "oledTask",
    .stack_size = 256 * 4,                       // 1024 bytes
    .priority   = (osPriority_t) osPriorityBelowNormal,
};

osThreadId_t flashTaskHandle;
const osThreadAttr_t flashTask_attributes = {
    .name       = "flashTask",
    .stack_size = 256 * 4,                       // 1024 bytes
    .priority   = (osPriority_t) osPriorityLow,
};

void app_init(void)
{
	oledMutexHandle = osMutexNew(NULL);

	uartQueueHandle = osMessageQueueNew(32, sizeof(uint8_t), NULL);
	logQueueHandle  = osMessageQueueNew(16, sizeof(System_Data_t), NULL);

	cliTaskHandle   = osThreadNew(StartCliTask,   NULL, &cliTask_attributes);
	bmeTaskHandle   = osThreadNew(StartBmeTask,   NULL, &bmeTask_attributes);
	voltTaskHandle  = osThreadNew(StartVoltTask,  NULL, &voltTask_attributes);
	oledTaskHandle  = osThreadNew(StartOledTask,  NULL, &oledTask_attributes);
	flashTaskHandle = osThreadNew(StartFlashTask, NULL, &flashTask_attributes);
}
