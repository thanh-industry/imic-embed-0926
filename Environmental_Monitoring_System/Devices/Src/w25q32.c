#include "w25q32.h"

/* Biến lưu trữ cấu hình ngoại vi và chân điều khiển CS */
static W25Q32_HandleTypedef flash;

/* Mảng toàn cục lưu 4 byte JEDEC ID phục vụ quan sát trên Live Expressions */
uint8_t test_flash_raw_id[4] = {0};

/* --- Các hàm nội bộ điều khiển chân Chip Select (CS) --- */

static void w25q32_cs_low(void)
{
    HAL_GPIO_WritePin(flash.cs_port, flash.cs_pin, GPIO_PIN_RESET);
}

static void w25q32_cs_high(void)
{
    HAL_GPIO_WritePin(flash.cs_port, flash.cs_pin, GPIO_PIN_SET);
}

/* --- Các hàm chức năng chính --- */

HAL_StatusTypeDef w25q32_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin)
{
    if (hspi == NULL || cs_port == NULL || cs_pin == 0)
    {
        return HAL_ERROR;
    }

    flash.hspi = hspi;
    flash.cs_port = cs_port;
    flash.cs_pin = cs_pin;

    /* Đảm bảo chân CS ở mức HIGH ban đầu */
    w25q32_cs_high();
    HAL_Delay(5); // Thời gian chờ t_PUW sau khi cấp nguồn ổn định

    uint8_t tx_buf[4] = {W25Q32_CMD_READ_JEDEC_ID, 0xFF, 0xFF, 0xFF};
    uint8_t rx_buf[4] = {0};
    HAL_StatusTypeDef status;

    /* Giao tiếp SPI Full-Duplex: Gửi 1 byte lệnh + 3 dummy byte và nhận về 4 byte */
    w25q32_cs_low();
    status = HAL_SPI_TransmitReceive(flash.hspi, tx_buf, rx_buf, 4, 100);
    w25q32_cs_high();

    if (status != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Lưu giá trị thực tế đọc về vào mảng theo dõi */
    test_flash_raw_id[0] = rx_buf[0]; // Byte phản hồi khi gửi lệnh (bỏ qua)
    test_flash_raw_id[1] = rx_buf[1]; // Manufacturer ID (Winbond: 0xEF, GigaDevice: 0xC8)
    test_flash_raw_id[2] = rx_buf[2]; // Memory Type (thường là 0x40)
    test_flash_raw_id[3] = rx_buf[3]; // Capacity (0x16 cho dung lượng 32Mbit = 4MB)

    /* Kiểm tra tính hợp lệ của mã định danh chip */
    if ((rx_buf[1] != 0xEF && rx_buf[1] != 0xC8) || rx_buf[2] != 0x40 || rx_buf[3] != 0x16)
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

HAL_StatusTypeDef w25q32_wait_busy(uint32_t timeout_ms)
{
    uint32_t start_time = HAL_GetTick();
    uint8_t tx_data[2] = {W25Q32_CMD_READ_STATUS_REG1, 0xFF};
    uint8_t rx_data[2] = {0};

    while (1)
    {
        w25q32_cs_low();
        HAL_StatusTypeDef status = HAL_SPI_TransmitReceive(flash.hspi, tx_data, rx_data, 2, 100);
        w25q32_cs_high();

        if (status != HAL_OK)
        {
            return HAL_ERROR;
        }

        /* Bit 0 (BUSY) bằng 0: Chip đã hoàn tất chu kỳ ghi/xóa */
        if ((rx_data[1] & W25Q32_SR1_BUSY) == 0)
        {
            break;
        }

        if ((HAL_GetTick() - start_time) >= timeout_ms)
        {
            return HAL_TIMEOUT;
        }
    }

    return HAL_OK;
}

HAL_StatusTypeDef w25q32_erase_sector(uint32_t sector_address)
{
    uint8_t tx_buffer[4] = {0};

    tx_buffer[0] = W25Q32_CMD_SECTOR_ERASE;
    tx_buffer[1] = (uint8_t)((sector_address >> 16) & 0xFF);
    tx_buffer[2] = (uint8_t)((sector_address >> 8) & 0xFF);
    tx_buffer[3] = (uint8_t)(sector_address & 0xFF);

    /* Bật quyền ghi trước khi thực thi lệnh xóa */
    if (w25q32_write_enable() != HAL_OK)
    {
        return HAL_ERROR;
    }

    w25q32_cs_low();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 100);
    w25q32_cs_high();

    if (status != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Chờ xóa sector hoàn tất (Datasheet khuyến nghị tối đa 400-800ms) */
    return w25q32_wait_busy(1000);
}

HAL_StatusTypeDef w25q32_read(uint32_t address, uint8_t *buffer, uint32_t length)
{
    if (buffer == NULL || length == 0)
    {
        return HAL_ERROR;
    }

    uint8_t tx_buffer[4] = {0};
    tx_buffer[0] = W25Q32_CMD_READ_DATA;
    tx_buffer[1] = (uint8_t)((address >> 16) & 0xFF);
    tx_buffer[2] = (uint8_t)((address >> 8) & 0xFF);
    tx_buffer[3] = (uint8_t)(address & 0xFF);

    w25q32_cs_low();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 100);
    if (status != HAL_OK)
    {
        w25q32_cs_high();
        return HAL_ERROR;
    }

    /* Xóa sạch byte rác cặn trong thanh ghi nhận DR trước khi đọc chuỗi dữ liệu */
    __HAL_SPI_CLEAR_OVRFLAG(flash.hspi);

    status = HAL_SPI_Receive(flash.hspi, buffer, (uint16_t)length, 1000);
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

    if (w25q32_write_enable() != HAL_OK)
    {
        return HAL_ERROR;
    }

    uint8_t tx_buffer[4] = {0};
    tx_buffer[0] = W25Q32_CMD_PAGE_PROGRAM;
    tx_buffer[1] = (uint8_t)((address >> 16) & 0xFF);
    tx_buffer[2] = (uint8_t)((address >> 8) & 0xFF);
    tx_buffer[3] = (uint8_t)(address & 0xFF);

    w25q32_cs_low();
    HAL_StatusTypeDef status = HAL_SPI_Transmit(flash.hspi, tx_buffer, 4, 1000);
    if (status != HAL_OK)
    {
        w25q32_cs_high();
        return HAL_ERROR;
    }

    status = HAL_SPI_Transmit(flash.hspi, (uint8_t *)buffer, length, 1000);
    w25q32_cs_high();

    if (status != HAL_OK)
    {
        return HAL_ERROR;
    }

    /* Chờ ghi 1 trang hoàn tất (Datasheet tối đa ~3ms) */
    return w25q32_wait_busy(10);
}

HAL_StatusTypeDef w25q32_write(uint32_t address, const uint8_t *buffer, uint32_t length)
{
    if (buffer == NULL || length == 0)
    {
        return HAL_ERROR;
    }

    uint16_t page_offset = address % W25Q32_PAGE_SIZE;
    uint16_t bytes_to_write = W25Q32_PAGE_SIZE - page_offset;

    while (length > 0)
    {
        if (bytes_to_write > length)
        {
            bytes_to_write = (uint16_t)length;
        }

        if (w25q32_write_page(address, buffer, bytes_to_write) != HAL_OK)
        {
            return HAL_ERROR;
        }

        address += bytes_to_write;
        buffer += bytes_to_write;
        length -= bytes_to_write;

        /* Các lần ghi kế tiếp luôn bắt đầu từ đầu trang mới */
        bytes_to_write = W25Q32_PAGE_SIZE;
    }

    return HAL_OK;
}
