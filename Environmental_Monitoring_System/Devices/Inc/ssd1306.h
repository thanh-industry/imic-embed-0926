#ifndef __SSD1306_H
#define __SSD1306_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "stm32f4xx_hal.h"

//Define
#define SSD1306_I2C_ADDRESS 										(0X3C << 1)

#define SSD1306_WIDTH           128
#define SSD1306_HEIGHT          64
#define SSD1306_BUFFER_SIZE     ((SSD1306_WIDTH * SSD1306_HEIGHT) / 8) // 1024 bytes

/* Định nghĩa màu điểm ảnh */
#define SSD1306_COLOR_BLACK     0x00 // Tắt điểm ảnh
#define SSD1306_COLOR_WHITE     0x01 // Bật sáng điểm ảnh

#define SET_LOWER_COLUMN_START_ADDRESS_FOR_PAGE_ADDRESSING_MODE		0x00 //0x00 ~ 0x0F
#define SET_HIGHER_COLUMN_START_ADDRESS_FOR_PAGE_ADDRESSING_MODE	0x10 //0x10 ~ 0x1F
#define SET_MEMORY_ADDRESS_MODE 									0x20
#define SET_COLUMN_ADDRESS											0X21
#define SET_PAGE_ADDRESS											0x22
#define SET_DISPLAY_START_LINE										0x40 //0x40 ~ 0x7F
#define SET_CONTRAST_CONTROL_FOR_BANK0								0x81
#define SET_CHARGE_PUMP												0x8D

#define SET_SEGMENT_REMAP_NORMAL									0xA0
#define SET_SEGMENT_REMAP_INVERSE									0xA1
#define SET_ENTIRE_DISPLAY_RESUME									0xA4
#define SET_ENTIRE_DISPLAY_ON										0xA5
#define SET_NORMAL_DISPLAY											0xA6
#define SET_INVERSE_DISPLAY											0xA7
#define SET_MULTIPLEX_RATIO											0xA8
#define SET_DISPLAY_OFF												0xAE
#define SET_DISPLAY_ON												0xAF

#define SET_PAGE_START_ADDRESS_FOR_PAGE_ADDRESSING_MODE				0xB0 //0xB0 ~ 0xB7
#define SET_COM_OUTPUT_SCAN_DIRECTION_NORMAL						0xC0
#define SET_COM_OUTPUT_SCAN_DIRECTION_REMAP							0xC8
#define SET_DISPLAY_OFFSET											0xD3
#define SET_DISPLAY_CLOCK_DIVIDE_RATIO_OR_OSCILLATOR_FREQUENCY		0xD5
#define SET_PRE_CHARGE_PERIOD										0xD9
#define SET_COM_PINS_HARDWARE_CONFIGURATION							0xDA
#define SET_VCOMH_DESELECT_LEVEL									0xDB
#define NOP															0xE3
#define HORIZONTAL_SCROLL_SETUP_RIGHT								0x26
#define HORIZONTAL_SCROLL_SETUP_LEFT								0x27
#define CONTINUOUS_VERTICAL_AND_HORIZONTAL_SCROLL_SETUP_RIGHT		0x29
#define CONTINUOUS_VERTICAL_AND_HORIZONTAL_SCROLL_SETUP_LEFT		0x2A
#define DEACTIVATE_SCROLL											0x2E
#define ACTIVATE_SCROLL												0x2F
#define SET_VERTICAL_SCROLL_AREA									0xA3

//Functions prototype

	/**
	 * @brief 	Hàm gửi các Control bytes
	 * @param 	hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @param 	cmd chọn command được sử dụng trên ssd1306
	 * @return 	HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef ssd1306_write_command(I2C_HandleTypeDef *hi2c, uint8_t cmd);

	/**
	 * @brief 	Khởi tạo ssd1306
	 * @param 	hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @return 	HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef ssd1306_init(I2C_HandleTypeDef *hi2c);

	/**
	 * @brief 	Hàm truyền data bytes chứa thông điệp cho ssd1306
	 * @param 	hi2c Con trỏ trỏ tới khối ngoại vi i2c trên stm32
	 * @return 	HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef ssd1306_update_screen	(I2C_HandleTypeDef *hi2c);

	/**
	 * @brief 	Làm sạch màn hình console
	 * @param 	color Bật hoặc tắt toàn bộ điểm ảnh
	 */
	void ssd1306_clear(uint8_t color);

	/**
	 * @brief 	Hàm sắp xếp các bytes dữ liệu đúng vị trí để xuất được dữ liệu mong muốn lên ssd1306
	 * @param 	x Tọa độ tương ứng với SEG
	 * @param 	y Tọa độ tương ứng với COM
	 * @param 	color Điểm ảnh bật/tắt
	 */
	void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color);

	/**
	 * @brief 	Hàm viết 1 ký tự
	 * @param 	x Tọa độ tương ứng với SEG
	 * @param 	y Tọa độ tương ứng với COM
	 * @param 	color Điểm ảnh bật/tắt
	 * @param 	ch Ký tự muốn vẽ
	 */
	void ssd1306_write_char(uint8_t x, uint8_t y, char ch, uint8_t color);

	/**
	 *@ brief 	Hàm viết 1 chuỗi ký tự
	 *@ param 	x Tọa độ tương ứng với SEG
	 *@ param 	y Tọa độ tương ứng với COM
	 *@ param 	color điểm ảnh bật/tắt
	 *@ param	str Chuỗi ký tự muốn vẽ
	 */
	void ssd1306_write_string(uint8_t x, uint8_t y, const char *str, uint8_t color);

#ifdef __cplusplus
}
#endif

#endif /* __SSD1306_H*/
