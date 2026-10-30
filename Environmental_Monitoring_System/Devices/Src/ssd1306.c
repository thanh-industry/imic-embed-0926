//Include
#include "ssd1306.h"
#include "ssd1306_fonts.c"
#include <stdio.h>

//Value

//static dùng bên trong 1 hàm (biến cục bộ - local scope) khi dùng biến cục bộ thông thường năm trên stack,
//mỗi lần gọi hàm thì nó lại cấp phát số lượng byte nhất định cho biến cục bộ, sau đó khi kết thúc hàm nó sẽ
//xóa toàn bộ số byte đã cấp cho biến cục bộ, nếu dùng static với biến cục bộ thì ta sẽ lưu biến cục bộ trên
//Flash (ROM) khi đó nó sẽ được lưu vĩnh viễn ở đó trong suốt quá trình mcu chạy, không cần tạo mới hay xóa đi
//sau mỗi lần gọi hàm.
//
//static dùng bên ngoài hàm (biến toàn cục - file scope), thì khi sử dụng như vậy tức là ta sẽ cấm các file khác
//sử dụng lại cùng tên với biến toàn cục đó.


//Funtions

HAL_StatusTypeDef ssd1306_write_command(I2C_HandleTypeDef *hi2c, uint8_t cmd)
{
	HAL_StatusTypeDef status;
	status = HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDRESS, 0x00, I2C_MEMADD_SIZE_8BIT, &cmd, 1, 100);
	if(status != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}

HAL_StatusTypeDef ssd1306_init(I2C_HandleTypeDef *hi2c)
{
	HAL_StatusTypeDef status;

	static uint8_t temp_buf[] = {

			//Cụm A: Nhóm Phần cứng tấm kính (Cố định theo kích thước  128 x 64)
			0xAE, 0xA8, 0x3F, 0xD3, 0x00, 0x40, 0xA1, 0xC8, 0xDA, 0x12,

			//Cụm B: Nhóm Nguồn và Xung nhịp (Dùng giá trị khuyến nghị của Datasheet)
			0xD5, 0x80, 0x8D, 0x14, 0xD9, 0xF1, 0xDB, 0x20, 0x81, 0xCF,

			//Cụm C: Nhóm Đưa vào hoạt động (Display Entry)
			0x20, 0x00, 0xA4, 0xA6, 0xAF
	};

	for(int i = 0; i < sizeof(temp_buf); i++)
	{
		status = ssd1306_write_command(hi2c, temp_buf[i]);
		if(status != HAL_OK)
		{
			return status;
		}
	}

	return HAL_OK;

}

HAL_StatusTypeDef ssd1306_update_screen	(I2C_HandleTypeDef *hi2c)
{

}

void ssd1306_clear(uint8_t color)
{

}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{

}

void ssd1306_write_char(uint8_t x, uint8_t y, char ch, uint8_t color)
{

}

void ssd1306_write_string(uint8_t x, uint8_t y, const char *str, uint8_t color)
{

}
