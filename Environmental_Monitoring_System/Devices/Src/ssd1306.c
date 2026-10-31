//Include
#include "ssd1306.h"
#include "ssd1306_fonts.h"
#include <stdio.h>

//Value
static uint8_t ssd1306_buffer[SSD1306_BUFFER_SIZE];

//Khai báo 2 biến này để sử dụng trong việc xác định vị trí bắt đầu của ký tự muốn in
static uint8_t CurrentX = 0;
static uint8_t CurrentY = 0;

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

	//Khi khai báo với const trình biên dịch sẽ chuyển hẳn 25 byte này sang lưu tại Flash (ROM), tiết kiệm được 25 byte SRAM quý giá cho vi điều khiển.
	static const uint8_t temp_buf[] = {

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
	HAL_StatusTypeDef status;

	//Set column address
	status = ssd1306_write_command(hi2c, SET_COLUMN_ADDRESS);
	if(status != HAL_OK)
	{
		return status;
	}

	status = ssd1306_write_command(hi2c, 0x00);
	if(status != HAL_OK)
	{
		return status;
	}

	status = ssd1306_write_command(hi2c, (SSD1306_WIDTH - 1));
	if(status != HAL_OK)
	{
		return status;
	}


	//Set page address
	status = ssd1306_write_command(hi2c, SET_PAGE_ADDRESS);
	if(status != HAL_OK)
	{
		return status;
	}

	status = ssd1306_write_command(hi2c, 0x00);
	if(status != HAL_OK)
	{
		return status;
	}

	status = ssd1306_write_command(hi2c, ((SSD1306_HEIGHT / 8) - 1));
	if(status != HAL_OK)
	{
		return status;
	}

	status = HAL_I2C_Mem_Write(hi2c, SSD1306_I2C_ADDRESS, 0x40, I2C_MEMADD_SIZE_8BIT, ssd1306_buffer, SSD1306_BUFFER_SIZE, 1000);
	if(status != HAL_OK)
	{
		return status;
	}

	return HAL_OK;
}

void ssd1306_clear(uint8_t color)
{
	uint8_t clear = 0x00;
	if(color == SSD1306_COLOR_WHITE)
	{
		clear = 0xFF;
	}
	for(int i = 0; i < SSD1306_BUFFER_SIZE; i++)
	{
		ssd1306_buffer[i] = clear;
	}
}

void ssd1306_draw_pixel(uint8_t x, uint8_t y, uint8_t color)
{
	if (x >= SSD1306_WIDTH || y >= SSD1306_HEIGHT)
	{
		return;
	}

	uint16_t index = x + ((y / 8) * SSD1306_WIDTH);
	if (color == SSD1306_COLOR_WHITE)
	    {
	        ssd1306_buffer[index] |= (1 << (y % 8));   // Bật bit lên 1
	    }
	    else
	    {
	        ssd1306_buffer[index] &= ~(1 << (y % 8));  // Xóa bit về 0
	    }

}

void ssd1306_set_cursor(uint8_t x, uint8_t y)
{
    CurrentX = x;
    CurrentY = y;
}

void ssd1306_write_char(char ch, SSD1306_Font_t font, uint8_t color)
{
	if(ch < 32 || ch > 126)
	{
		return;
	}
	if((CurrentX + font.width) > SSD1306_WIDTH || (CurrentY + font.height) > SSD1306_HEIGHT)
	{
		return;
	}

	uint16_t char_index = ch - 32; //Xác định vị trí của ký tự trong bảng tra font
	uint16_t char_start = char_index * font.height; //Vị trí bắt đầu của ký tự đó, ví dụ ký tự sp thì có phải là 32 - 32 = 0 => khi đó vị trí bắt đầu là 0 (0*font.height = 0)
									//tương tự ví dụ khác: ! là 33 - 32 = 1 => khi đó *font.height nữa có phải là vị trí bắt đầu là 8 không? (ở đây đang lấy ví dụ 6x8 pixel)

	for(int i = 0; i < font.height; i++)
	{
		uint16_t line = font.data[char_start + i];
		for(int j = 0; j < font.width; j++)
		{
			if((line << j) & 0x8000)
			{
				ssd1306_draw_pixel(CurrentX + j, CurrentY + i, color);
			}
			else
			{
				ssd1306_draw_pixel(CurrentX + j, CurrentY + i, !color);
			}
		}
	}
	CurrentX += font.width;
}

void ssd1306_write_string(const char *str, SSD1306_Font_t font, uint8_t color)
{
	if(str == NULL) return;

	int index = 0;
	while(str[index] != '\0')
	{
		ssd1306_write_char(str[index], font, color);
		index++;
	}
}
