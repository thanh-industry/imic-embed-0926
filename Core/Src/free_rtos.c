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
#include "spi_w25qxx.h"
#include "i2c_bmp280.h"
#include "i2c_oled_lcd.h"
#include <string.h>
#include <stdio.h>
#include <math.h> // THÊM THƯ VIỆN NÀY ĐỂ DÙNG HÀM fabs()
/* USER CODE END Includes */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef hspi1;
extern I2C_HandleTypeDef hi2c1;

float g_current_voltage = 0.0f;
float g_temperature     = 28.2f;
uint8_t g_i2c_status    = 1;
uint8_t g_spi_status    = 1;
uint32_t g_system_tick  = 0;

osMutexId_t uartMutexHandle;
osMutexId_t dataMutexHandle;

const osMutexAttr_t uartMutex_attributes = { .name = "uartMutex", .attr_bits = osMutexPrioInherit };
const osMutexAttr_t dataMutex_attributes = { .name = "dataMutex", .attr_bits = osMutexPrioInherit };

osMessageQueueId_t uartQueueHandle;
const osMessageQueueAttr_t uartQueue_attributes = { .name = "uartQueue" };
/* USER CODE END Variables */

/* Task Attributes */
osThreadId_t adcTaskHandle;
const osThreadAttr_t adcTask_attributes = { .name = "adcTask", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };
osThreadId_t cmdTaskHandle;
const osThreadAttr_t cmdTask_attributes = { .name = "cmdTask", .stack_size = 512 * 4, .priority = (osPriority_t) osPriorityNormal };
osThreadId_t sensorTaskHandle;
const osThreadAttr_t sensorTask_attributes = { .name = "sensorTask", .stack_size = 512 * 4, .priority = (osPriority_t) osPriorityNormal }; // Đã tăng Stack
osThreadId_t spiTaskHandle;
const osThreadAttr_t spiTask_attributes = { .name = "spiTask", .stack_size = 256 * 4, .priority = (osPriority_t) osPriorityNormal };

/* Function Prototypes */
void StartADCTask(void *argument);
void StartCommandTask(void *argument);
void StartSensorTask(void *argument);
void StartSPITask(void *argument);

void MX_FREERTOS_Init(void) {
  uartMutexHandle = osMutexNew(&uartMutex_attributes);
  dataMutexHandle = osMutexNew(&dataMutex_attributes);
  uartQueueHandle = osMessageQueueNew(128, sizeof(uint8_t), &uartQueue_attributes);

  adcTaskHandle = osThreadNew(StartADCTask, NULL, &adcTask_attributes);
  cmdTaskHandle = osThreadNew(StartCommandTask, NULL, &cmdTask_attributes);
  sensorTaskHandle = osThreadNew(StartSensorTask, NULL, &sensorTask_attributes);
  spiTaskHandle = osThreadNew(StartSPITask, NULL, &spiTask_attributes);
}

/* USER CODE BEGIN Application */

/* 1. ADCTask: Đọc điện áp */
void StartADCTask(void *argument) {
  LOG_I("[ADC] ADCTask started.");
  for(;;) {
    float temp_voltage = ADC_Read_Filtered_Voltage(10);
    osMutexAcquire(dataMutexHandle, osWaitForever);
    g_current_voltage = temp_voltage;
    g_system_tick++;
    osMutexRelease(dataMutexHandle);
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

/* 2. CommandTask: Nhận lệnh UART */
void StartCommandTask(void *argument) {
  uint8_t rx_byte;
  char cmd_buffer[32];
  uint8_t index = 0;

  LOG_I("[CMD] CommandTask ready. Go lenh va an Enter.");

  for(;;) {
    if (osMessageQueueGet(uartQueueHandle, &rx_byte, NULL, osWaitForever) == osOK) {
      if(rx_byte == '\r' || rx_byte == '\n') {
        if(index > 0) {
          cmd_buffer[index] = '\0';
          if(strcmp(cmd_buffer, "help") == 0) {
            LOG_I("--- HELP ---");
            LOG_I("  help, status, voltage");
          }
          else if(strcmp(cmd_buffer, "status") == 0) {
            osMutexAcquire(dataMutexHandle, osWaitForever);
            float local_v = g_current_voltage; float local_t = g_temperature;
            uint32_t local_tick = g_system_tick; uint8_t local_spi = g_spi_status;
            osMutexRelease(dataMutexHandle);

            LOG_I("=== System Status ===");
            LOG_I("Voltage     : %d.%02d V", (int)local_v, (int)((local_v - (int)local_v) * 100));
            LOG_I("Temperature : %d.%d C", (int)local_t, (int)((local_t - (int)local_t) * 10));
            LOG_I("SPI Device  : %s", local_spi ? "OK" : "ERROR");
            LOG_I("System Tick : %lu", local_tick);
          }
          else if(strcmp(cmd_buffer, "voltage") == 0) {
            osMutexAcquire(dataMutexHandle, osWaitForever);
            float local_v = g_current_voltage;
            osMutexRelease(dataMutexHandle);
            LOG_I("Voltage: %d.%02d V", (int)local_v, (int)((local_v - (int)local_v) * 100));
          }
          else {
            LOG_W("Unknown: '%s'", cmd_buffer);
          }
          index = 0;
        }
      }
      else if (rx_byte == 0x08 || rx_byte == 0x7F) { // Backspace
          if (index > 0) {
              index--;
              HAL_UART_Transmit(&huart2, (uint8_t*)"\b \b", 3, 10);
          }
      }
      else if(index < sizeof(cmd_buffer) - 1) {
        cmd_buffer[index++] = rx_byte;
        HAL_UART_Transmit(&huart2, &rx_byte, 1, 10);
      }
    }
  }
}

/* 3. SensorTask: Đọc cảm biến và hiển thị OLED (Bản tối ưu) */
void StartSensorTask(void *argument) {
  LOG_I("[I2C] SensorTask started.");

  float last_temp = -100.0f;

  for(;;) {
    float temp_val = BMP280_ReadTemperature();

    osMutexAcquire(dataMutexHandle, osWaitForever);
    g_temperature = temp_val;
    g_i2c_status = (temp_val > -40.0f && temp_val < 85.0f) ? 1 : 0;
    osMutexRelease(dataMutexHandle);

    if (temp_val > -40.0f && temp_val < 85.0f) {
        // Sử dụng hàm fabs() để tính giá trị tuyệt đối (đã include math.h)
        if (fabs(temp_val - last_temp) > 0.1f) {
            char buffer[20];
            snprintf(buffer, sizeof(buffer), "Temp: %.1f C", temp_val);
            OLED_ShowString(0, 2, buffer);
            OLED_UpdateScreen();
            last_temp = temp_val;
        }
    } else {
        static uint8_t warn_count = 0;
        if (warn_count < 3) {
            LOG_W("Loi cam bien BMP280: %.1f", temp_val);
            warn_count++;
        }
    }

    vTaskDelay(pdMS_TO_TICKS(2000));
  }
}

/* 4. SPITask: Kiểm tra chip nhớ */
void StartSPITask(void *argument) {
  LOG_I("[SPI] SPITask started.");
  for(;;) {
    uint32_t id = W25Q_ReadID();
    osMutexAcquire(dataMutexHandle, osWaitForever);
    if (id != 0x000000 && id != 0xFFFFFF) {
        g_spi_status = 1;
        LOG_I("[SPI] W25Q ID: 0x%06lX (OK)", id);
    } else {
        g_spi_status = 0;
        LOG_W("[SPI] W25Q ID: 0x%06lX (ERROR)", id);
    }
    osMutexRelease(dataMutexHandle);
    vTaskDelay(pdMS_TO_TICKS(5000));
  }
}

void vApplicationMallocFailedHook(void) { LOG_E("[FATAL] HET RAM (Heap)!"); for(;;); }
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) { LOG_E("[FATAL] TRAN STACK tai task: %s", pcTaskName); for(;;); }
/* USER CODE END Application */
