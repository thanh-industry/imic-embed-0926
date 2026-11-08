#include "task_storage.h"
#include "app_main.h"
#include "w25q32.h"
#include "main.h"

extern SPI_HandleTypeDef hspi1;

void StartFlashTask(void *argument)
{
    uint32_t current_write_addr = STORAGE_START_ADDR;
    HAL_StatusTypeDef status;

    /* 1. Khởi tạo chip nhớ W25Q32 qua SPI1 (CS: PA4) */
    status = w25q32_init(&hspi1, GPIOA, GPIO_PIN_4);
    if (status == HAL_OK)
    {
        current_sys_data.flash_ok = 1;
        /* Xóa sạch Sector đầu tiên sẵn sàng nhận dữ liệu */
        w25q32_erase_sector(current_write_addr);
    }
    else
    {
        current_sys_data.flash_ok = 0;
    }

    for (;;)
    {
        System_Data_t rx_data;

        /* 2. Chờ dữ liệu từ queue (tự ngủ khi không có gói tin) */
        if (osMessageQueueGet(logQueueHandle, &rx_data, NULL, STORAGE_QUEUE_TIMEOUT_MS) == osOK)
        {
            if (current_sys_data.flash_ok == 1)
            {
                /* Kiểm tra nếu khoảng trống còn lại của Sector không đủ chứa 1 gói tin */
                uint32_t offset_in_sector = current_write_addr % W25Q32_SECTOR_SIZE;
                if ((W25Q32_SECTOR_SIZE - offset_in_sector) < sizeof(System_Data_t))
                {
                    /* Nhảy sang đầu Sector kế tiếp */
                    current_write_addr = current_write_addr - offset_in_sector + W25Q32_SECTOR_SIZE;

                    /* Kiểm tra giới hạn vùng nhớ (10 Sector) để quay vòng (Ring Buffer) */
                    if (current_write_addr >= (STORAGE_START_ADDR + 10 * W25Q32_SECTOR_SIZE))
                    {
                        current_write_addr = STORAGE_START_ADDR;
                    }

                    /* Xóa Sector mới trước khi bắt đầu ghi */
                    w25q32_erase_sector(current_write_addr);
                }

                /* Ghi gói tin vào Flash */
                w25q32_write(current_write_addr, (const uint8_t *)&rx_data, sizeof(System_Data_t));
                current_write_addr += sizeof(System_Data_t);
            }
        }
        /* Tuyệt đối không để osDelay ở đây để tránh task bị ngủ đông */
    }
}
