/* USER CODE BEGIN Header */
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"
/* USER CODE END Header */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "free_rtos.h"
#include "logging.h"
#include "adc.h"
#include "spi_w25qxx.h"
#include "i2c_bmp280.h"
#include "i2c_oled_lcd.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct {
    float    voltage;
    float    temperature;
    uint8_t  i2c_status;
    uint8_t  spi_status;
    uint32_t adc_sample_count;
} SystemData_t;

typedef void (*CmdHandler_t)(void);

typedef struct {
    const char  *name;
    CmdHandler_t handler;
} CommandDef_t;

typedef struct {
    uint32_t     hash;
    const char  *name;
    CmdHandler_t handler;
} HashSlot_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* Kich thuoc bo dem va hang doi */
#define CMD_BUFFER_SIZE          32U
#define VOLTAGE_OUT_BUF_SIZE     32U
#define STATUS_OUT_BUF_SIZE      256U
#define OLED_LINE_BUF_SIZE       20U
#define UART_QUEUE_LENGTH        256U

/* Ky tu ASCII hop le */
#define ASCII_PRINTABLE_MIN      0x20U
#define ASCII_PRINTABLE_MAX      0x7EU
#define ASCII_BS                 0x08U
#define ASCII_DEL                0x7FU

/* Thoi gian cho (Timeout) */
#define DATA_MUTEX_TIMEOUT_MS    100U
#define UART_MUTEX_TIMEOUT_MS    50U
#define UART_TX_TIMEOUT_MS       10U
#define CMD_RX_TIMEOUT_MS        1000U
#define CMD_IDLE_RESET_SEC       5U

/* Chu ky thuc thi cac Task (ms) */
#define ADC_TASK_PERIOD_MS       500U
#define SENSOR_TASK_PERIOD_MS    2000U
#define SPI_TASK_PERIOD_MS       5000U

/* Nguong va tham so cam bien / ngoai vi */
#define ADC_FILTER_SAMPLES       10U
#define BMP280_TEMP_MIN_C        (-40.0f)
#define BMP280_TEMP_MAX_C        (85.0f)
#define OLED_TEMP_DEADBAND_C     (0.1f)
#define SENSOR_MAX_WARN_COUNT    3U
#define W25Q_ID_INVALID_LOW      0x000000UL
#define W25Q_ID_INVALID_HIGH     0xFFFFFFUL

/* Kich thuoc Stack tinh theo words (1 word = 4 bytes tren ARM Cortex-M) */
#define ADC_TASK_STACK_WORDS     256U
#define CMD_TASK_STACK_WORDS     512U
#define SNS_TASK_STACK_WORDS     512U
#define SPI_TASK_STACK_WORDS     256U

/* Cau hinh Static Hash Table (Kich thuoc la luy thua cua 2) */
#define HASH_TABLE_SIZE          16U
#define HASH_TABLE_MASK          (HASH_TABLE_SIZE - 1U)
#define FNV1A_32_OFFSET_BASIS    2166136261UL
#define FNV1A_32_PRIME           16777619UL

/* Ma loi he thong ghi nhan khi gap su co nghiem trong */
#define FAULT_INIT_SYNC_OBJ      0x00000001UL
#define FAULT_INIT_TASK_OBJ      0x00000002UL
#define FAULT_HASH_TABLE_FULL    0x00000003UL
#define FAULT_MALLOC_FAILED      0xDEAD0001UL
#define FAULT_STACK_OVERFLOW     0xDEAD0002UL
/* USER CODE END PD */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
extern UART_HandleTypeDef huart2;
extern SPI_HandleTypeDef  hspi1;
extern I2C_HandleTypeDef  hi2c1;
#ifdef HAL_IWDG_MODULE_ENABLED
extern IWDG_HandleTypeDef hiwdg;
#endif

static SystemData_t s_sys_data = {
    .voltage          = 0.0f,
    .temperature      = 28.2f,
    .i2c_status       = 1U,
    .spi_status       = 1U,
    .adc_sample_count = 0U
};

static HashSlot_t        s_commandHashTable[HASH_TABLE_SIZE];
static volatile uint8_t  s_hash_table_ready = 0U;
static volatile uint32_t s_fatal_fault_code = 0UL;
volatile uint32_t        g_uart_drop_count  = 0UL;// đếm byte bị tràn ra ngoài khi queue đầy

/* --- Vung nho tinh cho Mutex & Queue (100% khong dung Heap) --- */
static StaticSemaphore_t s_uartMutexControlBlock;// khóa khi uart dung
static StaticSemaphore_t s_dataMutexControlBlock;//khóa data
static StaticQueue_t     s_uartQueueControlBlock;// chứa chuỗi chính
static uint8_t           s_uartQueueStorage[UART_QUEUE_LENGTH * sizeof(uint8_t)];// bộ đệm nhận kí tự
// khai báo biến MUTEX
osMutexId_t uartMutexHandle = NULL;
osMutexId_t dataMutexHandle = NULL;

const osMutexAttr_t uartMutex_attributes = {
    .name      = "uartMutex",
    .attr_bits = osMutexPrioInherit,
    .cb_mem    = &s_uartMutexControlBlock,
    .cb_size   = sizeof(s_uartMutexControlBlock)
};

const osMutexAttr_t dataMutex_attributes = {
    .name      = "dataMutex",
    .attr_bits = osMutexPrioInherit,
    .cb_mem    = &s_dataMutexControlBlock,
    .cb_size   = sizeof(s_dataMutexControlBlock)
};

osMessageQueueId_t uartQueueHandle = NULL;// khai báo Queue
const osMessageQueueAttr_t uartQueue_attributes = {
    .name    = "uartQueue",
    .cb_mem  = &s_uartQueueControlBlock,
    .cb_size = sizeof(s_uartQueueControlBlock),
    .mq_mem  = s_uartQueueStorage,
    .mq_size = sizeof(s_uartQueueStorage)
};

/* --- Vung nho tinh TCB & Stack cho 4 Task (100% khong dung Heap) --- */
static StaticTask_t s_adcTaskTCB;
static StaticTask_t s_cmdTaskTCB;
static StaticTask_t s_sensorTaskTCB;
static StaticTask_t s_spiTaskTCB;

static StackType_t  s_adcTaskStack[ADC_TASK_STACK_WORDS];
static StackType_t  s_cmdTaskStack[CMD_TASK_STACK_WORDS];
static StackType_t  s_sensorTaskStack[SNS_TASK_STACK_WORDS];
static StackType_t  s_spiTaskStack[SPI_TASK_STACK_WORDS];

osThreadId_t adcTaskHandle    = NULL;
osThreadId_t cmdTaskHandle    = NULL;
osThreadId_t sensorTaskHandle = NULL;
osThreadId_t spiTaskHandle    = NULL;

const osThreadAttr_t adcTask_attributes = {
    .name       = "adcTask",
    .cb_mem     = &s_adcTaskTCB,
    .cb_size    = sizeof(s_adcTaskTCB),
    .stack_mem  = s_adcTaskStack,
    .stack_size = sizeof(s_adcTaskStack),
    .priority   = (osPriority_t) osPriorityNormal
};

const osThreadAttr_t cmdTask_attributes = {
    .name       = "cmdTask",
    .cb_mem     = &s_cmdTaskTCB,
    .cb_size    = sizeof(s_cmdTaskTCB),
    .stack_mem  = s_cmdTaskStack,
    .stack_size = sizeof(s_cmdTaskStack),
    .priority   = (osPriority_t) osPriorityNormal
};

const osThreadAttr_t sensorTask_attributes = {
    .name       = "sensorTask",
    .cb_mem     = &s_sensorTaskTCB,
    .cb_size    = sizeof(s_sensorTaskTCB),
    .stack_mem  = s_sensorTaskStack,
    .stack_size = sizeof(s_sensorTaskStack),
    .priority   = (osPriority_t) osPriorityNormal
};

const osThreadAttr_t spiTask_attributes = {
    .name       = "spiTask",
    .cb_mem     = &s_spiTaskTCB,
    .cb_size    = sizeof(s_spiTaskTCB),
    .stack_mem  = s_spiTaskStack,
    .stack_size = sizeof(s_spiTaskStack),
    .priority   = (osPriority_t) osPriorityNormal
};
/* USER CODE END Variables */

/* Private function prototypes -----------------------------------------------*/
static void     Cmd_HandleHelp(void);
static void     Cmd_HandleStatus(void);
static void     Cmd_HandleVoltage(void);
static uint32_t ComputeCommandHash(const char *str);
static uint8_t  InitCommandHashTable(void);
static void     DispatchCommand(const char *raw_cmd);
static uint8_t  UART_SafeTransmit(const uint8_t *data, uint16_t len);
static void     SplitFloatFixed(float val, uint32_t scale, uint8_t *is_neg, int32_t *int_part, int32_t *frac_part);

/* Danh sach lenh goc dat trong Flash (.rodata) */
static const CommandDef_t kCommandDefs[] = {
    { "help",    Cmd_HandleHelp    },
    { "status",  Cmd_HandleStatus  },
    { "voltage", Cmd_HandleVoltage }
};
#define COMMAND_DEF_COUNT (sizeof(kCommandDefs) / sizeof(kCommandDefs[0]))

/* Cap phat tinh cho Idle Task & Timer Task khi bat configSUPPORT_STATIC_ALLOCATION */
#if (configSUPPORT_STATIC_ALLOCATION == 1)
static StaticTask_t s_xIdleTaskTCBBuffer;
static StackType_t  s_xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer,
                                   StackType_t **ppxIdleTaskStackBuffer,
                                   uint32_t *pulIdleTaskStackSize) {
    *ppxIdleTaskTCBBuffer   = &s_xIdleTaskTCBBuffer;
    *ppxIdleTaskStackBuffer = &s_xIdleStack[0];
    *pulIdleTaskStackSize   = configMINIMAL_STACK_SIZE;
}

#if (configUSE_TIMERS == 1)
static StaticTask_t s_xTimerTaskTCBBuffer;
static StackType_t  s_xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer,
                                    StackType_t **ppxTimerTaskStackBuffer,
                                    uint32_t *pulTimerTaskStackSize) {
    *ppxTimerTaskTCBBuffer   = &s_xTimerTaskTCBBuffer;
    *ppxTimerTaskStackBuffer = &s_xTimerStack[0];
    *pulTimerTaskStackSize   = configTIMER_TASK_STACK_DEPTH;
}
#endif /* configUSE_TIMERS */
#endif /* configSUPPORT_STATIC_ALLOCATION */
static void StartADCTask(void *argument);
static void StartCommandTask(void *argument);
static void StartSensorTask(void *argument);
static void StartSPITask(void *argument);
// khai báo
void MX_FREERTOS_Init(void) {
    if (InitCommandHashTable() == 0U) {
        s_fatal_fault_code = FAULT_HASH_TABLE_FULL;
        Error_Handler();
    }

    uartMutexHandle = osMutexNew(&uartMutex_attributes);
    dataMutexHandle = osMutexNew(&dataMutex_attributes);
    uartQueueHandle = osMessageQueueNew(UART_QUEUE_LENGTH, sizeof(uint8_t), &uartQueue_attributes);

    if ((uartMutexHandle == NULL) || (dataMutexHandle == NULL) || (uartQueueHandle == NULL)) {
        s_fatal_fault_code = FAULT_INIT_SYNC_OBJ;
        Error_Handler();
    }

    adcTaskHandle    = osThreadNew(StartADCTask, NULL, &adcTask_attributes);
    cmdTaskHandle    = osThreadNew(StartCommandTask, NULL, &cmdTask_attributes);
    sensorTaskHandle = osThreadNew(StartSensorTask, NULL, &sensorTask_attributes);
    spiTaskHandle    = osThreadNew(StartSPITask, NULL, &spiTask_attributes);

    if ((adcTaskHandle == NULL) || (cmdTaskHandle == NULL) ||
        (sensorTaskHandle == NULL) || (spiTaskHandle == NULL)) {
        s_fatal_fault_code = FAULT_INIT_TASK_OBJ;
        Error_Handler();
    }
}

/* USER CODE BEGIN Application */

/* Ham goi tu HAL_UART_RxCpltCallback de day byte vao Queue va dem drop an toan */
void UART_RxByteFromISR(uint8_t rx_byte) {
    if (uartQueueHandle != NULL) {
        if (osMessageQueuePut(uartQueueHandle, &rx_byte, 0U, 0U) != osOK) {
            g_uart_drop_count++;
        }
    }
}

/* Tach phan nguyen va phan thap phan an toan (chan NaN/Inf, tranh -0.0, khong dung %f) */
static void SplitFloatFixed(float val, uint32_t scale, uint8_t *is_neg, int32_t *int_part, int32_t *frac_part) {
    float   abs_val;
    int32_t scaled_total;

    if ((is_neg == NULL) || (int_part == NULL) || (frac_part == NULL) || (scale == 0U)) {
        return;
    }

    if (isnan(val) || isinf(val)) {
        *is_neg    = 0U;
        *int_part  = 0;
        *frac_part = 0;
        return;
    }

    abs_val      = fabsf(val);
    scaled_total = (int32_t)((abs_val * (float)scale) + 0.5f);
    *is_neg      = ((val < 0.0f) && (scaled_total != 0)) ? 1U : 0U;
    *int_part    = scaled_total / (int32_t)scale;
    *frac_part   = scaled_total % (int32_t)scale;
}

/* Truyen UART co khoa uartMutex + timeout; tra ve 1 neu thanh cong, 0 neu ban/loi */
static uint8_t UART_SafeTransmit(const uint8_t *data, uint16_t len) {
    HAL_StatusTypeDef hal_st;

    if ((data == NULL) || (len == 0U) || (uartMutexHandle == NULL)) {
        return 0U;
    }

    if (osMutexAcquire(uartMutexHandle, pdMS_TO_TICKS(UART_MUTEX_TIMEOUT_MS)) == osOK)
    	//lấy thời gian chờ là số tick, nếu chờ lâu qua nó return
    {
        hal_st = HAL_UART_Transmit(&huart2, (uint8_t *)data, len, UART_TX_TIMEOUT_MS);
        configASSERT(osMutexRelease(uartMutexHandle) == osOK);
        return (hal_st == HAL_OK) ? 1U : 0U;
    }

    return 0U;
}

/* Ham bam FNV-1a 32-bit: do phuc tap chi phu thuoc do dai chuoi lenh */
static uint32_t ComputeCommandHash(const char *str) {
    uint32_t       hash = FNV1A_32_OFFSET_BASIS;
    const uint8_t *ptr  = (const uint8_t *)str;

    if (ptr == NULL) {
        return 0UL;
    }

    while (*ptr != '\0') {
        hash ^= (uint32_t)(*ptr);
        hash *= FNV1A_32_PRIME;
        ptr++;
    }
    return hash;
}

/* Nạp bảng băm tĩnh (Open Addressing + Linear Probing) một lần duy nhất lúc khởi động */
static uint8_t InitCommandHashTable(void) {
    uint32_t cmd_idx;    /* Chỉ số duyệt qua mảng định nghĩa lệnh kCommandDefs */
    uint32_t probe;      /* Bước nhảy dò tìm ô trống khi xảy ra đụng độ băm (collision) */
    uint32_t hash;       /* Giá trị mã băm 32-bit (ví dụ FNV-1a) tính từ tên lệnh */
    uint32_t slot_idx;   /* Chỉ số ô khởi đầu trong bảng băm sau khi áp dụng mask */
    uint8_t  inserted;   /* Cờ trạng thái: 1U = đã chèn thành công lệnh hiện tại, 0U = chưa chèn được */

    /* Xóa toàn bộ vùng nhớ của bảng băm tĩnh về 0 -> mọi slot->handler đều trở thành NULL (ô trống) */
    (void)memset(s_commandHashTable, 0, sizeof(s_commandHashTable));

    /* Duyệt tuần tự qua từng lệnh có sẵn trong bảng cấu hình tĩnh kCommandDefs */
    for (cmd_idx = 0U; cmd_idx < COMMAND_DEF_COUNT; cmd_idx++) {
        /* Tính mã băm 32-bit từ chuỗi tên lệnh (vd: "help", "status", "voltage") */
        hash     = ComputeCommandHash(kCommandDefs[cmd_idx].name);

        /* Ánh xạ hash (0..2^32-1) về chỉ số hợp lệ [0 .. HASH_TABLE_SIZE - 1] bằng bitmask */
        slot_idx = hash & HASH_TABLE_MASK;

        /* Reset cờ chèn trước khi bắt đầu dò vị trí cho lệnh cmd_idx */
        inserted = 0U;

        /* Linear probing: thử tối đa HASH_TABLE_SIZE ô liên tiếp bắt đầu từ slot_idx */
        for (probe = 0U; probe < HASH_TABLE_SIZE; probe++) {
            /* Lấy địa chỉ ô cần kiểm tra; (slot_idx + probe) & HASH_TABLE_MASK giúp vòng lại về 0 khi chạm cuối mảng */
            HashSlot_t *slot = &s_commandHashTable[(slot_idx + probe) & HASH_TABLE_MASK];
            if (slot->handler == NULL) {
                slot->hash    = hash;                        /* Lưu sẵn hash để tra cứu nhanh và so sánh trước khi strcmp */
                slot->name    = kCommandDefs[cmd_idx].name;    /* Lưu con trỏ tên lệnh để đối chiếu chính xác chuỗi */
                slot->handler = kCommandDefs[cmd_idx].handler; /* Lưu con trỏ hàm callback xử lý lệnh */
                inserted      = 1U;                          /* Đánh dấu lệnh này đã được nạp vào bảng */
                break;                                       /* Thoát vòng lặp dò probe vì đã tìm được chỗ */
            }
            /* Nếu slot->handler != NULL nghĩa là ô này đã có lệnh khác -> tiếp tục vòng probe kế tiếp */
        }

        /* Nếu đã dò hết bảng mà vẫn không có slot->handler == NULL -> bảng băm đầy (tràn bảng) */
        if (inserted == 0U) {
            s_hash_table_ready = 0U; /* Đánh dấu bảng băm không sẵn sàng / khởi tạo lỗi */
            return 0U;               /* Trả về 0 báo thất bại */
        }
    }

    /* Đã nạp toàn bộ COMMAND_DEF_COUNT lệnh vào bảng thành công */
    s_hash_table_ready = 1U; /* Bật cờ báo hiệu bảng băm hợp lệ, có thể dùng trong DispatchCommand */
    return 1U;               /* Trả về 1 báo thành công */
}
/* Tra cuu va thuc thi lenh O(1) trung binh qua bang bam tinh */
static void DispatchCommand(const char *raw_cmd) {
    uint32_t cmd_hash;
    uint32_t slot_idx;
    uint32_t probe;

    if ((raw_cmd == NULL) || (s_hash_table_ready == 0U)) {
        return;
    }

    cmd_hash = ComputeCommandHash(raw_cmd);
    slot_idx = cmd_hash & HASH_TABLE_MASK;

    for (probe = 0U; probe < HASH_TABLE_SIZE; probe++) {
        const HashSlot_t *slot = &s_commandHashTable[(slot_idx + probe) & HASH_TABLE_MASK];

        if (slot->handler == NULL)// KHÔNG CÓ LÀ NGƯNG
        {
            break;
        }

        if ((slot->hash == cmd_hash) && (strcmp(slot->name, raw_cmd) == 0)) {
            slot->handler();
            return;
        }
    }

    LOG_W("Unknown: '%s'", raw_cmd);
}

static void Cmd_HandleHelp(void) {
    static const char kHelpText[] = "--- HELP ---\r\n  help, status, voltage\r\n";
    if (UART_SafeTransmit((const uint8_t *)kHelpText, (uint16_t)(sizeof(kHelpText) - 1U)) == 0U) {
        LOG_W("[CMD] UART ban, khong gui duoc help.");
    }
}

static void Cmd_HandleStatus(void) {
    /* Chi duoc goi tuan tu trong CommandTask -> dung static de tiet kiem 256B Stack */
    static char  out_buf[STATUS_OUT_BUF_SIZE];
    SystemData_t local_snapshot;
    int          len;
    uint16_t     tx_len;
    uint8_t      v_neg;
    uint8_t      t_neg;
    int32_t      v_int;
    int32_t      v_frac;
    int32_t      t_int;
    int32_t      t_frac;

    if (dataMutexHandle == NULL) {
        return;
    }

    if (osMutexAcquire(dataMutexHandle, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS)) == osOK) {
        local_snapshot = s_sys_data;
        configASSERT(osMutexRelease(dataMutexHandle) == osOK);

        SplitFloatFixed(local_snapshot.voltage, 100U, &v_neg, &v_int, &v_frac);
        SplitFloatFixed(local_snapshot.temperature, 10U, &t_neg, &t_int, &t_frac);

        len = snprintf(out_buf, sizeof(out_buf),
                       "=== System Status ===\r\n"
                       "Voltage     : %s%ld.%02ld V\r\n"
                       "Temperature : %s%ld.%ld C\r\n"
                       "I2C Device  : %s\r\n"
                       "SPI Device  : %s\r\n"
                       "ADC Samples : %lu\r\n"
                       "UART Drops  : %lu\r\n",
                       (v_neg != 0U) ? "-" : "", (long)v_int, (long)v_frac,
                       (t_neg != 0U) ? "-" : "", (long)t_int, (long)t_frac,
                       (local_snapshot.i2c_status != 0U) ? "OK" : "ERROR",
                       (local_snapshot.spi_status != 0U) ? "OK" : "ERROR",
                       (unsigned long)local_snapshot.adc_sample_count,
                       (unsigned long)g_uart_drop_count);

        if (len > 0) {
            tx_len = (len < (int)sizeof(out_buf))
                     ? (uint16_t)len
                     : (uint16_t)(sizeof(out_buf) - 1U);
            if (UART_SafeTransmit((const uint8_t *)out_buf, tx_len) == 0U) {
                LOG_W("[CMD] UART ban, khong gui duoc status.");
            }
            if (len >= (int)sizeof(out_buf)) {
                LOG_W("[CMD] Output status bi cat bot (%d >= %u).", len, (unsigned int)sizeof(out_buf));
            }
        }
    } else {
        LOG_W("[CMD] Mutex ban, bo qua lenh status.");
    }
}

static void Cmd_HandleVoltage(void) {
    float    local_voltage;
    char     out_buf[VOLTAGE_OUT_BUF_SIZE];
    int      len;
    uint16_t tx_len;
    uint8_t  v_neg;
    int32_t  v_int;
    int32_t  v_frac;

    if (dataMutexHandle == NULL) {
        return;
    }

    if (osMutexAcquire(dataMutexHandle, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS)) == osOK) {
        local_voltage = s_sys_data.voltage;
        configASSERT(osMutexRelease(dataMutexHandle) == osOK);

        SplitFloatFixed(local_voltage, 100U, &v_neg, &v_int, &v_frac);
        len = snprintf(out_buf, sizeof(out_buf),
                       "Voltage: %s%ld.%02ld V\r\n",
                       (v_neg != 0U) ? "-" : "", (long)v_int, (long)v_frac);

        if (len > 0) {
            tx_len = (len < (int)sizeof(out_buf))
                     ? (uint16_t)len
                     : (uint16_t)(sizeof(out_buf) - 1U);
            if (UART_SafeTransmit((const uint8_t *)out_buf, tx_len) == 0U) {
                LOG_W("[CMD] UART ban, khong gui duoc voltage.");
            }
            if (len >= (int)sizeof(out_buf)) {
                LOG_W("[CMD] Output voltage bi cat bot (%d >= %u).", len, (unsigned int)sizeof(out_buf));
            }
        }
    } else {
        LOG_W("[CMD] Mutex ban, thu lai sau.");
    }
}

/* 1. ADCTask: Doc dien ap */
void StartADCTask(void *argument) {
    float temp_voltage;
    (void)argument;

    if (dataMutexHandle == NULL) {
        vTaskDelete(NULL);
        return;
    }

    LOG_I("[ADC] ADCTask started.");
    for (;;) {
        temp_voltage = ADC_Read_Filtered_Voltage(ADC_FILTER_SAMPLES);

        if (osMutexAcquire(dataMutexHandle, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS)) == osOK) {
            s_sys_data.voltage = temp_voltage;
            s_sys_data.adc_sample_count++;
            configASSERT(osMutexRelease(dataMutexHandle) == osOK);
        } else {
            LOG_W("[ADC] Mutex timeout, bo qua lan cap nhat nay.");
        }

        vTaskDelay(pdMS_TO_TICKS(ADC_TASK_PERIOD_MS));
    }
}

/* 2. CommandTask: Nhan lenh UART co timeout, khong bao gio treo vo han */
void StartCommandTask(void *argument) {
    uint8_t    rx_byte;
    char       cmd_buffer[CMD_BUFFER_SIZE];
    uint16_t   index        = 0U;
    uint8_t    idle_seconds = 0U;
    uint8_t    echo_lost    = 0U;
    osStatus_t q_status;
    (void)argument;

    if ((uartQueueHandle == NULL) || (dataMutexHandle == NULL) || (uartMutexHandle == NULL)) {
        vTaskDelete(NULL);
        return;
    }

    LOG_I("[CMD] CommandTask ready. Go lenh va an Enter.");
    for (;;) {
        q_status = osMessageQueueGet(uartQueueHandle, &rx_byte, NULL, pdMS_TO_TICKS(CMD_RX_TIMEOUT_MS));

        if (q_status == osOK) {
            idle_seconds = 0U;

            if ((rx_byte == '\r') || (rx_byte == '\n')) {
                if (index > 0U) {
                    cmd_buffer[index] = '\0';
                    (void)UART_SafeTransmit((const uint8_t *)"\r\n", 2U);
                    if (echo_lost != 0U) {
                        LOG_W("[CMD] UART ban luc go, co ky tu bi mat echo.");
                        echo_lost = 0U;
                    }
                    DispatchCommand(cmd_buffer);
                    index = 0U;
                }
            } else if ((rx_byte == ASCII_BS) || (rx_byte == ASCII_DEL)) {
                if (index > 0U) {
                    index--;
                    if (UART_SafeTransmit((const uint8_t *)"\b \b", 3U) == 0U) {
                        echo_lost = 1U;
                    }
                }
            } else if ((rx_byte >= ASCII_PRINTABLE_MIN) && (rx_byte <= ASCII_PRINTABLE_MAX)) {
                if (index < (CMD_BUFFER_SIZE - 1U)) {
                    cmd_buffer[index++] = (char)rx_byte;
                    if (UART_SafeTransmit(&rx_byte, 1U) == 0U) {
                        echo_lost = 1U;
                    }
                } else {
                    index     = 0U;
                    echo_lost = 0U;
                    LOG_W("[CMD] Lenh qua dai (>%u ky tu), da huy.", (unsigned int)(CMD_BUFFER_SIZE - 1U));
                }
            } else {
                /* Bo qua ky tu dieu khien khong in duoc */
            }
        } else if (q_status == osErrorTimeout) {
#ifdef HAL_IWDG_MODULE_ENABLED
            (void)HAL_IWDG_Refresh(&hiwdg);
#endif
            if (index > 0U) {
                idle_seconds++;
                if (idle_seconds >= CMD_IDLE_RESET_SEC) {
                    index        = 0U;
                    idle_seconds = 0U;
                    echo_lost    = 0U;
                    LOG_W("[CMD] Het thoi gian nhap lenh, da xoa bo dem.");
                }
            }
        }
    }
}

/* 3. SensorTask: Doc cam bien BMP280 va hien thi OLED (100% khong dung %f) */
void StartSensorTask(void *argument) {
    float   temp_val;
    float   last_temp  = -100.0f;
    uint8_t is_valid;
    uint8_t warn_count = 0U;
    uint8_t t_neg;
    int32_t t_int;
    int32_t t_frac;
    char    oled_buffer[OLED_LINE_BUF_SIZE];
    (void)argument;

    if (dataMutexHandle == NULL) {
        vTaskDelete(NULL);
        return;
    }

    LOG_I("[I2C] SensorTask started.");
    for (;;) {
        temp_val = BMP280_ReadTemperature();
        is_valid = ((temp_val > BMP280_TEMP_MIN_C) && (temp_val < BMP280_TEMP_MAX_C)) ? 1U : 0U;

        if (osMutexAcquire(dataMutexHandle, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS)) == osOK) {
            s_sys_data.temperature = temp_val;
            s_sys_data.i2c_status  = is_valid;
            configASSERT(osMutexRelease(dataMutexHandle) == osOK);
        } else {
            LOG_W("[I2C] Mutex timeout, chua cap nhat duoc nhiet do.");
        }

        if (is_valid != 0U) {
            warn_count = 0U;
            if (fabsf(temp_val - last_temp) > OLED_TEMP_DEADBAND_C) {
                SplitFloatFixed(temp_val, 10U, &t_neg, &t_int, &t_frac);
                (void)snprintf(oled_buffer, sizeof(oled_buffer),
                               "Temp: %s%ld.%ld C",
                               (t_neg != 0U) ? "-" : "", (long)t_int, (long)t_frac);
                OLED_ShowString(0, 2, oled_buffer);
                OLED_UpdateScreen();
                last_temp = temp_val;
            }
        } else if (warn_count < SENSOR_MAX_WARN_COUNT) {
            LOG_W("[I2C] Loi cam bien BMP280.");
            warn_count++;
        }

        vTaskDelay(pdMS_TO_TICKS(SENSOR_TASK_PERIOD_MS));
    }
}

/* 4. SPITask: Kiem tra chip nho Flash W25Qxx */
void StartSPITask(void *argument) {
    uint32_t flash_id;
    uint8_t  is_ok;
    (void)argument;

    if (dataMutexHandle == NULL) {
        vTaskDelete(NULL);
        return;
    }

    LOG_I("[SPI] SPITask started.");
    for (;;) {
        flash_id = W25Q_ReadID();
        is_ok = ((flash_id != W25Q_ID_INVALID_LOW) && (flash_id != W25Q_ID_INVALID_HIGH)) ? 1U : 0U;

        if (osMutexAcquire(dataMutexHandle, pdMS_TO_TICKS(DATA_MUTEX_TIMEOUT_MS)) == osOK) {
            s_sys_data.spi_status = is_ok;
            configASSERT(osMutexRelease(dataMutexHandle) == osOK);
        } else {
            LOG_W("[SPI] Mutex timeout, chua cap nhat trang thai SPI.");
        }

        if (is_ok != 0U) {
            LOG_I("[SPI] W25Q ID: 0x%06lX (OK)", (unsigned long)flash_id);
        } else {
            LOG_W("[SPI] W25Q ID: 0x%06lX (ERROR)", (unsigned long)flash_id);
        }

        vTaskDelay(pdMS_TO_TICKS(SPI_TASK_PERIOD_MS));
    }
}

/* Hook loi nghiem trong: KHONG goi UART/printf trong hook de tranh deadlock ngat */
void vApplicationMallocFailedHook(void) {
    taskDISABLE_INTERRUPTS();
    s_fatal_fault_code = FAULT_MALLOC_FAILED;
    for (;;);
}

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
    (void)xTask;
    (void)pcTaskName;
    taskDISABLE_INTERRUPTS();
    s_fatal_fault_code = FAULT_STACK_OVERFLOW;
    for (;;);
}
