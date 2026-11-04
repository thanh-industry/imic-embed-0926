#include "test_hardware.h"
#include "main.h"

/* Include 5 driver tầng Device */
#include "adc_sensor.h"
#include "rtc_time.h"
#include "bme280.h"
#include "w25q32.h"
#include "ssd1306.h"

#include <string.h>

/* Khai báo extern font hiển thị cho OLED */
#if __has_include("fonts.h")
#include "fonts.h"
#else
extern SSD1306_Font_t Font_7x10;
#endif

/* Khai báo extern các ngoại vi phần cứng từ main.c */
extern ADC_HandleTypeDef hadc1;
extern RTC_HandleTypeDef hrtc;
extern I2C_HandleTypeDef hi2c1; /* Dành riêng cho BME280 */
extern I2C_HandleTypeDef hi2c2; /* Dành riêng cho SSD1306 OLED */
extern SPI_HandleTypeDef hspi1;

/* --- BIẾN QUAN SÁT TRÊN LIVE EXPRESSIONS --- */
typedef struct {
    uint8_t adc_ok;
    uint8_t rtc_ok;
    uint8_t bme280_ok;
    uint8_t w25q32_ok;
    uint8_t display_ok;
} Hardware_Status_t;

Hardware_Status_t hw_status = {0};

/* 1. Biến ADC */
volatile uint16_t test_adc_raw = 0;
volatile float    test_adc_voltage = 0.0f;

/* 2. Biến RTC */
RTC_DateTime_t test_rtc_dt;
char           test_rtc_str[RTC_STRING_BUFFER_SIZE];

/* 3. Biến BME280 */
uint8_t     test_bme_chip_id = 0;
bme280_data test_bme_data = {0};

/* 4. Biến Flash W25Q32 */
uint8_t test_flash_tx[16] = "FLASH_TEST_OK!";
uint8_t test_flash_rx[16] = {0};
uint8_t test_flash_verify_ok = 0;

void test_hardware_init(void)
{
    /* 1. Test ADC (ADC1_IN1) */
    adc_sensor_init(&hadc1);
    hw_status.adc_ok = 1;

    /* 2. Test RTC */
    if (rtc_time_init(&hrtc) == HAL_OK) {
        hw_status.rtc_ok = 1;
    }

    /* 3. Test BME280 (Giao tiếp qua I2C1 - Địa chỉ 0x76) */
    if (bme280_init(&hi2c1) == HAL_OK) {
        bme280_read_id(&hi2c1, &test_bme_chip_id);
        if (test_bme_chip_id == 0x60) {
            hw_status.bme280_ok = 1;
        }
    }

    /* 4. Test W25Q32 (Giao tiếp qua SPI1) */
    if (w25q32_init(&hspi1, SPI1_CS_GPIO_Port, SPI1_CS_Pin) == HAL_OK) {
        /* Xóa sector 0 -> Ghi chuỗi kiểm tra -> Đọc lại đối soát */
        w25q32_erase_sector(0x000000);
        w25q32_write(0x000000, test_flash_tx, sizeof(test_flash_tx));
        w25q32_read(0x000000, test_flash_rx, sizeof(test_flash_rx));

        if (memcmp(test_flash_tx, test_flash_rx, sizeof(test_flash_tx)) == 0) {
            test_flash_verify_ok = 1;
            hw_status.w25q32_ok = 1;
        }
    }

    /* 5. Test SSD1306 OLED (Giao tiếp qua I2C2) */
    if (ssd1306_init(&hi2c2) == HAL_OK) {
        hw_status.display_ok = 1;
        ssd1306_clear(SSD1306_COLOR_BLACK);
        ssd1306_set_cursor(0, 0);
        #if __has_include("fonts.h")
        ssd1306_write_string("SYSTEM BRINGUP", Font_7x10, SSD1306_COLOR_WHITE);
        #endif
        ssd1306_update_screen(&hi2c2);
    }
}

void test_hardware_run(void)
{
    /* 1. Đọc ADC lọc 16 mẫu */
    test_adc_raw = adc_sensor_read_raw();
    test_adc_voltage = adc_sensor_read_filtered_voltage(16);

    /* 2. Đọc RTC */
    rtc_time_get_val(&test_rtc_dt);
    rtc_time_get_string(test_rtc_str);

    /* 3. Đọc dữ liệu môi trường từ BME280 qua I2C1 */
    sensor_get_vals(&hi2c1, &test_bme_data);

    /* 4. Hiển thị RTC lên màn hình OLED qua I2C2 */
    if (hw_status.display_ok) {
        ssd1306_clear(SSD1306_COLOR_BLACK);
        ssd1306_set_cursor(0, 0);
        #if __has_include("fonts.h")
        ssd1306_write_string(test_rtc_str, Font_7x10, SSD1306_COLOR_WHITE);
        #endif
        ssd1306_update_screen(&hi2c2);
    }

    HAL_Delay(1000);
}
