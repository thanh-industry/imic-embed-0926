/* spi_w25qxx.c */
#include "spi_w25qxx.h"
 //khai báo
static SPI_HandleTypeDef *w25q_spi = NULL;

/**
 * @brief  Kéo chân CS xuống mức LOW (0V) để bắt đầu giao tiếp với chip.
 *         Theo chuẩn SPI, chip chỉ lắng nghe khi chân CS = 0.
 */
static void W25Q_Select(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_RESET);
}

/**
 * @brief  Kéo chân CS lên mức HIGH (3.3V) để kết thúc giao tiếp.
 *         Chip sẽ bỏ qua mọi tín hiệu trên đường SPI cho đến lần Select tiếp theo
 */
static void W25Q_Deselect(void) {
    HAL_GPIO_WritePin(W25Q_CS_PORT, W25Q_CS_PIN, GPIO_PIN_SET);
}

/**
 * @brief  Khởi tạo driver.
 * @param  hspi: Con trỏ trỏ đến cấu hình SPI (VD: &hspi1).
 */
void W25Q_Init(SPI_HandleTypeDef *hspi) {
    w25q_spi = hspi;       // Lưu lại cấu hình SPI để dùng cho các hàm sau
    W25Q_Deselect();       // Đảm bảo chip ở trạng thái nghỉ ban đầu
}

/**
 * @brief  Đọc mã định danh JEDEC ID của chip (VD: 0xEF4016).
 * @retval Mã ID 32-bit.
 */
uint32_t W25Q_ReadID(void) {
    // Mảng tx chứa lệnh gửi đi: Lệnh Read ID (0x9F) + 3 byte rác (0)
    // Tại sao cần byte rác? Vì SPI là giao thức Full-duplex: muốn nhận 1 byte, phải gửi 1 byte.
    uint8_t tx[4] = {W25Q_JEDEC_ID, 0, 0, 0};
    uint8_t rx[4] = {0};   // Mảng hứng dữ liệu trả về

    W25Q_Select();         // Bắt đầu giao tiếp
    // Gửi 4 byte đồng thời nhận 4 byte trong 1 lần (tối ưu tốc độ)
    HAL_SPI_TransmitReceive(w25q_spi, tx, rx, 4, 100);
    W25Q_Deselect();       // Kết thúc giao tiếp

    // Ghép 3 byte ID (nằm ở rx[1], rx[2], rx[3]) thành 1 số 32-bit
    // Ví dụ: rx[1]=0xEF, rx[2]=0x40, rx[3]=0x16 -> Kết quả: 0xEF4016
    return ((uint32_t)rx[1] << 16) | ((uint32_t)rx[2] << 8) | rx[3];
}

/**
 * @brief  Đọc thanh ghi trạng thái (Status Register 1).
 * @retval Giá trị 8-bit của thanh ghi trạng thái.
 */
uint8_t W25Q_ReadStatus(void) {
    uint8_t tx[2] = {W25Q_READ_STATUS1, 0}; // Lệnh Read Status (0x05) + 1 byte rác
    uint8_t rx[2] = {0};

    W25Q_Select();
    HAL_SPI_TransmitReceive(w25q_spi, tx, rx, 2, 100);
    W25Q_Deselect();

    return rx[1]; // Byte trạng thái thực sự nằm ở rx[1]
}

/**
 * @brief  Chờ cho chip xử lý xong công việc hiện tại (ghi/xóa).
 *         Bit 0 của thanh ghi trạng thái là cờ BUSY (1 = đang bận, 0 = rảnh).
 */
void W25Q_WaitForReady(void) {
    // Liên tục đọc trạng thái cho đến khi bit 0 về 0
    while ((W25Q_ReadStatus() & 0x01) == 0x01);
}

/**
 * @brief  Đọc dữ liệu từ chip.
 * @param  addr: Địa chỉ bắt đầu đọc (24-bit).
 * @param  pData: Con trỏ mảng để chứa dữ liệu đọc được.
 * @param  len: Số byte cần đọc.
 */
void W25Q_ReadData(uint32_t addr, uint8_t *pData, uint32_t len) {
    // Tạo gói lệnh: Lệnh Read Data (0x03) + 3 byte địa chỉ
    // Tách địa chỉ 24-bit thành 3 byte bằng toán tử dịch bit
    uint8_t tx[4] = {W25Q_READ_DATA, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx, 4, 100);      // Gửi lệnh + địa chỉ (4 byte)
    HAL_SPI_Receive(w25q_spi, pData, len, 100);  // Nhận dữ liệu (len byte)
    W25Q_Deselect();
}

/**
 * @brief  Ghi dữ liệu vào chip.
 * @param  addr: Địa chỉ cần ghi.
 * @param  pData: Dữ liệu cần ghi.
 * @param  len: Số byte cần ghi.
 */
void W25Q_WriteData(uint32_t addr, uint8_t *pData, uint32_t len) {
    // Chip chỉ cho phép ghi tối đa 1 Page (256 byte) trong 1 lệnh
    if (len > W25Q_PAGE_SIZE) len = W25Q_PAGE_SIZE;

    // Bước 1: Gửi lệnh Write Enable (0x06) để "mở khóa" chip trước khi ghi
    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    // Bước 2: Gửi lệnh Page Program (0x02) + Địa chỉ (3 byte)
    uint8_t tx_header[4] = {W25Q_PAGE_PROGRAM, (uint8_t)(addr >> 16), (uint8_t)(addr >> 8), (uint8_t)addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx_header, 4, 100); // Gửi lệnh + địa chỉ
    HAL_SPI_Transmit(w25q_spi, pData, len, 100);   // Gửi dữ liệu thực tế
    W25Q_Deselect();

    // Bước 3: Chờ chip ghi xong (có thể mất vài mili-giây)
    W25Q_WaitForReady();
}

/**
 * @brief  Xóa 1 Sector (4KB) của chip.
 * @param  sector_addr: Địa chỉ của Sector cần xóa.
 */
void W25Q_EraseSector(uint32_t sector_addr) {
    // Bước 1: Write Enable
    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    // Bước 2: Gửi lệnh Sector Erase (0x20) + Địa chỉ
    uint8_t tx_header[4] = {W25Q_SECTOR_ERASE, (uint8_t)(sector_addr >> 16), (uint8_t)(sector_addr >> 8), (uint8_t)sector_addr};

    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, tx_header, 4, 100);
    W25Q_Deselect();

    // Bước 3: Chờ xóa xong
    W25Q_WaitForReady();
}

/**
 * @brief  Xóa toàn bộ chip (mất nhiều thời gian).
 */
void W25Q_EraseChip(void) {
    // Bước 1: Write Enable
    uint8_t cmd_enable = W25Q_WRITE_ENABLE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_enable, 1, 100);
    W25Q_Deselect();

    // Bước 2: Gửi lệnh Chip Erase (0xC7)
    uint8_t cmd_erase = W25Q_CHIP_ERASE;
    W25Q_Select();
    HAL_SPI_Transmit(w25q_spi, &cmd_erase, 1, 100);
    W25Q_Deselect();

    // Bước 3: Chờ xóa xong (có thể mất vài giây)
    W25Q_WaitForReady();
}
