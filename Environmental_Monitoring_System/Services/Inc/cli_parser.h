#ifndef CLI_PARSER_H
#define CLI_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "stm32f4xx.h"

//Define

//Struct
typedef void (*cli_func_t)(void);

typedef struct {
    const char *name;     // Tên lệnh (chuỗi string)
    const char *help;     // Câu mô tả (chuỗi string)
    cli_func_t function;  // Con trỏ trỏ tới hàm xử lý
} cli_command_t;

typedef struct {
    float   temperature;  // Nhiệt độ từ BME280 (VD: 28.2)
    float   humidity;     // Độ ẩm từ BME280
    float   pressure;     // Áp suất từ BME280
    float   voltage;      // Điện áp pin từ ADC (VD: 3.25)

    uint8_t bme280_ok;     // 1: Cảm biến BME280 (I2C1) OK, 0: Lỗi
	uint8_t oled_ok;       // 1: Màn hình OLED (I2C2) OK, 0: Lỗi
	uint8_t flash_ok;      // 1: Flash W25Q32 (SPI1) OK, 0: Lỗi
	uint8_t adc_ok;        // 1: ADC đọc tốt, 0: Lỗi
}	System_Data_t;

extern System_Data_t current_sys_data;

//Functions Prototype

	/**
	 * @brief Hàm để các Task đo đạc đẩy dữ liệu mới nhất vào cho CLI
	 */
	void cli_parser_update_data(const System_Data_t *data);

	/**
	 * @brief Hàm nhận và bóc tách chuỗi lệnh
	 */
	void cli_parser_process(char *cmd_line);

#ifdef __cplusplus
}
#endif

#endif /* _CLI_PARSER_H */
