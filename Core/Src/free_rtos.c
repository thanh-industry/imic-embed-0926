/* USER CODE BEGIN Header */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
/* USER CODE END Header */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "logging.h"
#include "adc.h"
#include "freertos.h"
#include <string.h>
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart2;

/* Biến toàn cục chia sẻ dữ liệu */
float g_current_voltage = 0.0f;
float g_temperature     = 28.2f;
uint8_t g_i2c_status    = 1;
uint8_t g_spi_status    = 1;
uint32_t g_system_tick  = 0;

/* Khai báo Mutex */
osMutexId_t uartMutexHandle;
osMutexId_t dataMutexHandle;

/* Cấu hình thuộc tính cho Mutex */
const osMutexAttr_t uartMutex_attributes = {
  .name = "uartMutex",
  .attr_bits = osMutexPrioInherit
};
const osMutexAttr_t dataMutex_attributes = {
  .name = "dataMutex",
  .attr_bits = osMutexPrioInherit
};

/* Khai báo Queue */
osMessageQueueId_t uartQueueHandle;
const osMessageQueueAttr_t uartQueue_attributes = {
  .name = "uartQueue"
};
/* USER CODE END Variables */

/* Definitions for adcTask */
osThreadId_t adcTaskHandle;
const osThreadAttr_t adcTask_attributes = {
  .name = "adcTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for cmdTask */
osThreadId_t cmdTaskHandle;
const osThreadAttr_t cmdTask_attributes = {
  .name = "cmdTask",
  .stack_size = 512 * 4, // 2KB
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for sensorTask */
osThreadId_t sensorTaskHandle;
const osThreadAttr_t sensorTask_attributes = {
  .name = "sensorTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Definitions for spiTask */
osThreadId_t spiTaskHandle;
const osThreadAttr_t spiTask_attributes = {
  .name = "spiTask",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */
void StartADCTask(void *argument);
void StartCommandTask(void *argument);
void StartSensorTask(void *argument);
void StartSPITask(void *argument);
/* USER CODE END FunctionPrototypes */

void MX_FREERTOS_Init(void);

void MX_FREERTOS_Init(void) {
  // 1. Tạo Mutex TRƯỚC KHI tạo Task
  uartMutexHandle = osMutexNew(&uartMutex_attributes);
  dataMutexHandle = osMutexNew(&dataMutex_attributes);

  // 2. Tạo Queue cho UART (Chứa tối đa 128 bytes)
  uartQueueHandle = osMessageQueueNew(128, sizeof(uint8_t), &uartQueue_attributes);

  // 3. Tạo các Task
  adcTaskHandle = osThreadNew(StartADCTask, NULL, &adcTask_attributes);
  cmdTaskHandle = osThreadNew(StartCommandTask, NULL, &cmdTask_attributes);
  sensorTaskHandle = osThreadNew(StartSensorTask, NULL, &sensorTask_attributes);
  spiTaskHandle = osThreadNew(StartSPITask, NULL, &spiTask_attributes);
}

/* USER CODE BEGIN Application */

/* 1. ADCTask: Chuyên đọc điện áp định kỳ */
void StartADCTask(void *argument)
{
  LOG_I("[ADC] ADCTask started.");
  for(;;)
  {
    // Đọc điện áp ra biến cục bộ trước (không giữ khóa trong lúc đọc ADC)
    float temp_voltage = ADC_Read_Filtered_Voltage(10);

    // Bắt đầu vùng tranh chấp (Critical Section) - Khóa dữ liệu
    osMutexAcquire(dataMutexHandle, osWaitForever);
    g_current_voltage = temp_voltage;
    g_system_tick++;
    osMutexRelease(dataMutexHandle); // Mở khóa

    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/* 2. CommandTask: Xử lý tập lệnh UART (Sử dụng Queue thay vì Blocking) */
void StartCommandTask(void *argument)
{
  uint8_t rx_byte;
  char cmd_buffer[32];
  uint8_t index = 0;

  LOG_I("[CMD] CommandTask ready. Type: help, status, voltage, sensor, version");

  for(;;)
  {
    // Chờ lấy 1 byte từ Queue. Nếu Queue rỗng, Task sẽ ngủ (Blocked).
    // osWaitForever = chờ mãi mãi cho đến khi có dữ liệu.
    if (osMessageQueueGet(uartQueueHandle, &rx_byte, NULL, osWaitForever) == osOK)
    {
      if(rx_byte == '\r' || rx_byte == '\n')
      {
        if(index > 0)
        {
          cmd_buffer[index] = '\0';
          index = 0; // Reset buffer

          // --- XỬ LÝ LỆNH ---
          if(strcmp(cmd_buffer, "help") == 0) {
            LOG_I("  help    - Hien thi danh sach lenh");
            LOG_I("  status  - Xem trang thai he thong");
            LOG_I("  voltage - Xem dien ap ADC");
          }
          else if(strcmp(cmd_buffer, "status") == 0)
          {
            // Khóa dữ liệu để đọc an toàn
            osMutexAcquire(dataMutexHandle, osWaitForever);
            float local_v = g_current_voltage;
            float local_t = g_temperature;
            uint32_t local_tick = g_system_tick;
            osMutexRelease(dataMutexHandle);

            // In ra ngoài vùng khóa để không làm nghẽn hệ thống
            LOG_I("=== System Status ===");
            LOG_I("Voltage     : %d.%02d V", (int)local_v, (int)((local_v - (int)local_v) * 100));
            LOG_I("Temperature : %d.%d C", (int)local_t, (int)((local_t - (int)local_t) * 10));
            LOG_I("System Tick : %lu", local_tick);
          }
          else if(strcmp(cmd_buffer, "voltage") == 0)
          {
            osMutexAcquire(dataMutexHandle, osWaitForever);
            float local_v = g_current_voltage;
            osMutexRelease(dataMutexHandle);

            LOG_I("[ADC] Voltage = %d.%02d V", (int)local_v, (int)((local_v - (int)local_v) * 100));
          }
          else
          {
            LOG_W("Unknown command: '%s'", cmd_buffer);
          }
        }
      }
      else if(index < sizeof(cmd_buffer) - 1)
      {
        cmd_buffer[index++] = rx_byte;
        // Echo lại ký tự vừa nhận
        HAL_UART_Transmit(&huart2, &rx_byte, 1, 10);
      }
    }
  }
}

/* 3. SensorTask: Đọc cảm biến I2C định kỳ */
void StartSensorTask(void *argument)
{
  LOG_I("[I2C] SensorTask started.");
  for(;;)
  {
    float temp_val = 28.2f; // Giả lập đọc cảm biến

    osMutexAcquire(dataMutexHandle, osWaitForever);
    g_temperature = temp_val;
    g_i2c_status = 1;
    osMutexRelease(dataMutexHandle);

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

/* 4. SPITask: Kiểm tra thiết bị SPI định kỳ */
void StartSPITask(void *argument)
{
  LOG_I("[SPI] SPITask started.");
  for(;;)
  {
    osMutexAcquire(dataMutexHandle, osWaitForever);
    g_spi_status = 1;
    osMutexRelease(dataMutexHandle);

    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

/* ===== HÀM HOOK BẮT LỖI ===== */
void vApplicationMallocFailedHook(void) {
    LOG_E("[FATAL] HET RAM (Heap)! Tang configTOTAL_HEAP_SIZE len!");
    for(;;);
}
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    LOG_E("[FATAL] TRAN STACK tai task: %s", pcTaskName);
    for(;;);
}
/* USER CODE END Application */
