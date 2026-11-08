#include "task_display.h"
#include "app_main.h"
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c2;

void StartOledTask(void *argument)
{
	char display_buf[32];
	HAL_StatusTypeDef status;

	status = ssd1306_init(&hi2c2);
	if(status == HAL_OK)
	{
		current_sys_data.oled_ok = 1;
	}
	else current_sys_data.oled_ok = 0;

	ssd1306_clear(SSD1306_COLOR_BLACK);

    for (;;)
    {
    	// Nếu CLI đang cần giữ màn hình hiển thị
		if (oled_hold_seconds > 0)
		{
			oled_hold_seconds--;
			osDelay(DISPLAY_REFRESH_PERIOD_MS);
			continue; // Bỏ qua lượt vẽ thông số mặc định
		}

    	// 1. Chiếm Mutex
    	osMutexAcquire(oledMutexHandle, osWaitForever);

    	// 2. Xóa bộ đệm cũ
    	ssd1306_clear(SSD1306_COLOR_BLACK);

    	// 3. Định dạng và ghi chuỗi dòng 1 (Nhiệt độ & Độ ẩm)
    	snprintf(display_buf, sizeof(display_buf), "Temp: %.1f C", current_sys_data.temperature);
    	ssd1306_set_cursor(0, 0);
    	ssd1306_write_string(display_buf, Font_7x10, SSD1306_COLOR_WHITE);

    	snprintf(display_buf, sizeof(display_buf), "Humi: %.1f %%", current_sys_data.humidity);
		ssd1306_set_cursor(0, 16);
		ssd1306_write_string(display_buf, Font_7x10, SSD1306_COLOR_WHITE);

		snprintf(display_buf, sizeof(display_buf), "Pres: %.0f hPa", current_sys_data.pressure);
		ssd1306_set_cursor(0, 32);
		ssd1306_write_string(display_buf, Font_7x10, SSD1306_COLOR_WHITE);

		snprintf(display_buf, sizeof(display_buf), "Voltage: %.2f V", current_sys_data.voltage);
		ssd1306_set_cursor(0, 48);
		ssd1306_write_string(display_buf, Font_7x10, SSD1306_COLOR_WHITE);

		ssd1306_update_screen(&hi2c2);

		osMutexRelease(oledMutexHandle);
        osDelay(DISPLAY_REFRESH_PERIOD_MS);
    }
}
