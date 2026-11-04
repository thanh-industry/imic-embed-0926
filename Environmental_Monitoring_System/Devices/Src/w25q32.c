//Include
#include "w25q32.h"

//Value
static W25Q32_HandleTypedef flash;
//Functions

static void w25q32_cs_low(void)
{
	HAL_GPIO_WritePin(flash.cs_port, flash.cs_pin, GPIO_PIN_RESET);
}

static void w25q32_cs_high(void)
{
	HAL_GPIO_WritePin(flash.cs_port, flash.cs_pin, GPIO_PIN_SET);
}

HAL_StatusTypeDef w25q32_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
	if(hspi == NULL || cs_port == NULL || cs_pin == 0)
	{
		return HAL_ERROR;
	}

	flash.hspi = hspi;
	flash.cs_port = cs_port;
	flash.cs_pin = cs_pin;

	w25q32_cs_high();
	HAL_Delay(5); //Theo datasheet, chip W25Q32 cần khoảng thời gian t_PUW (Power-up Time) để khối tạo cao áp bên trong ổn định trước khi nhận lệnh SPI đầu tiên.

	uint8_t cmd = W25Q32_CMD_READ_JEDEC_ID;
	uint8_t buffer[3] = {0};
	HAL_StatusTypeDef status;

	w25q32_cs_low();
	status = HAL_SPI_Transmit(hspi, &cmd, 1, 100);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}
	status = HAL_SPI_Receive(hspi, buffer, 3, 100);
	w25q32_cs_high();

	//Nếu trả về 0xEF, 0x40, 0x16 thì init w25q32 thành công
	if(buffer[0] != 0xEF || buffer[1] != 0x40 || buffer[2] != 0x16)
	{
		return HAL_ERROR;
	}

	return HAL_OK;

}

HAL_StatusTypeDef w25q32_write_enable(void)
{
	uint8_t cmd = W25Q32_CMD_WRITE_ENABLE;
	HAL_StatusTypeDef status;

	w25q32_cs_low();
	status = HAL_SPI_Transmit(flash.hspi, &cmd, 1, 100);
	w25q32_cs_high();

	return status;

}

HAL_StatusTypeDef w25q32_wait_busy(uint32_t timeout_ms) //Chúng ta phải đứng đợi đến khi nó xóa xong dữ liệu trước đó thì mới thực hiện tiếp thao tác ghi/xóa tiếp được.
{
	uint32_t start_time = HAL_GetTick();
	uint8_t cmd = W25Q32_CMD_READ_STATUS_REG1;
	uint8_t sr1 = 0;
	HAL_StatusTypeDef status;

	w25q32_cs_low();
	status = HAL_SPI_Transmit(flash.hspi, &cmd, 1, 100);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}

	while(1)
	{
		status = HAL_SPI_Receive(flash.hspi, &sr1, 1, 100);
		if(status != HAL_OK)
		{
			w25q32_cs_high();
			return HAL_ERROR;
		}

		//Kiểm tra bit bận
		if((sr1 & W25Q32_SR1_BUSY) == 0)
		{
			break;
		}

		//Kiểm tra thời gian tối đa nằm trong hàm
		if((HAL_GetTick() - start_time) >= (timeout_ms))
		{
			w25q32_cs_high();
			return HAL_ERROR;
		}
	}
	w25q32_cs_high();

	return HAL_OK;
}

HAL_StatusTypeDef w25q32_erase_sector(uint32_t sector_address)
{
	uint8_t tx_buffer[4] = {0};

	tx_buffer[0] = W25Q32_CMD_SECTOR_ERASE;
	tx_buffer[1] = (uint8_t) ((sector_address >> 16) & 0xFF);
	tx_buffer[2] = (uint8_t) ((sector_address >> 8) & 0xFF);
	tx_buffer[3] = (uint8_t) (sector_address & 0xFF);

	HAL_StatusTypeDef status;

	status = w25q32_write_enable();
	if(status != HAL_OK)
	{
		return HAL_ERROR;
	}

	w25q32_cs_low();
	status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 100);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}
	w25q32_cs_high();
	return w25q32_wait_busy(1000); //Cho phép tối đa 1000ms
}

HAL_StatusTypeDef w25q32_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
	if(buffer == NULL || length == 0)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status;

	uint8_t tx_buffer[4] = {0};
	tx_buffer[0] = W25Q32_CMD_READ_DATA;
	tx_buffer[1] = (uint8_t)((address >> 16) & 0xFF);
	tx_buffer[2] =	(uint8_t)((address >> 8 ) & 0xFF);
	tx_buffer[3] = (address & 0xFF);

	w25q32_cs_low();
	status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 100);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}

	status = HAL_SPI_Receive(flash.hspi, buffer, (uint16_t) length, 1000);

	w25q32_cs_high();
	return status;
}

HAL_StatusTypeDef w25q32_write_page(uint32_t address, const uint8_t *buffer, uint32_t length)
{
	uint16_t offset = address % W25Q32_PAGE_SIZE;
	if (buffer == NULL || length == 0 || (offset + length) > W25Q32_PAGE_SIZE)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status;

	if(w25q32_write_enable() != HAL_OK)
	{
		return HAL_ERROR;
	}

	uint8_t tx_buffer[4] = {0};
	tx_buffer[0] = W25Q32_CMD_PAGE_PROGRAM;
	tx_buffer[1] = (uint8_t)((address >> 16) & 0xFF);
	tx_buffer[2] = (uint8_t)((address >> 8 ) & 0xFF);
	tx_buffer[3] = (address & 0xFF);

	w25q32_cs_low();
	status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 1000);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}

	status = HAL_SPI_Transmit(flash.hspi,(uint8_t *) buffer, length, 1000);
	if(status != HAL_OK)
	{
		w25q32_cs_high();
		return HAL_ERROR;
	}

	w25q32_cs_high();
	return w25q32_wait_busy(10); // Chờ tối đa 10ms cho việc ghi 1 trang

	//Lưu ý: quá trình xóa lâu hơn quá trình ghi khá nhiều
}

HAL_StatusTypeDef w25q32_write(uint32_t address, const uint8_t *buffer, uint32_t length)
{
	if(buffer == NULL || length == 0)
	{
		return HAL_ERROR;
	}

	uint16_t page_offset = address % W25Q32_PAGE_SIZE;
	uint16_t bytes_to_write = W25Q32_PAGE_SIZE - page_offset;

	while (length > 0)
	{
		// Nếu số byte còn lại ít hơn khoảng trống của trang hiện tại thì chỉ ghi phần còn lại
		if (bytes_to_write > length)
		{
			bytes_to_write = (uint16_t)length;
		}

		// Ghi an toàn vào trang hiện tại
		if (w25q32_write_page(address, buffer, bytes_to_write) != HAL_OK)
		{
			return HAL_ERROR;
		}

		// Tịnh tiến các con trỏ và giảm dung lượng còn lại
		address += bytes_to_write;
		buffer += bytes_to_write;
		length -= bytes_to_write;

		// Từ trang thứ 2 trở đi, địa chỉ luôn nằm ngay đầu trang mới (offset = 0)
		// nên ta được phép ghi tối đa trọn vẹn 256 bytes
		bytes_to_write = W25Q32_PAGE_SIZE;
	}
	return HAL_OK;
}
