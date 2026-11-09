#ifndef __W25Q32_H
#define __W25Q32_H

#ifdef __cplusplus
extern "C" {
#endif

//Include
#include "stm32f4xx_hal.h"

//Define

/* Kích thước bộ nhớ W25Q32JV */
#define W25Q32_PAGE_SIZE                            256
#define W25Q32_SECTOR_SIZE                          4096
#define W25Q32_BLOCK_SIZE                           65536
#define W25Q32_FLASH_SIZE                           (4 * 1024 * 1024)

/* Mặt nạ bit thanh ghi trạng thái 1 (Status Register-1 Mask) */
#define W25Q32_SR1_BUSY                             0x01
#define W25Q32_SR1_WEL                              0x02

//1. Nhóm lệnh cơ sở điều khiển & Đọc/Ghi (Standard SPI)
#define W25Q32_CMD_WRITE_ENABLE                     0x06
#define W25Q32_CMD_WRITE_DISABLE                    0x04
#define W25Q32_CMD_READ_DATA                        0x03
#define W25Q32_CMD_FAST_READ                        0x0B
#define W25Q32_CMD_PAGE_PROGRAM                     0x02
#define W25Q32_CMD_SECTOR_ERASE                     0x20  /* 4KB */
#define W25Q32_CMD_BLOCK_ERASE_32KB                 0x52
#define W25Q32_CMD_BLOCK_ERASE_64KB                 0xD8
#define W25Q32_CMD_CHIP_ERASE                       0xC7  /* Hoặc 0x60 */

//2. Nhóm lệnh thanh ghi trạng thái (Status Registers)
#define W25Q32_CMD_READ_STATUS_REG1                 0x05
#define W25Q32_CMD_READ_STATUS_REG2                 0x35
#define W25Q32_CMD_READ_STATUS_REG3                 0x15
#define W25Q32_CMD_WRITE_STATUS_REG1                0x01
#define W25Q32_CMD_WRITE_STATUS_REG2                0x31
#define W25Q32_CMD_WRITE_STATUS_REG3                0x11
#define W25Q32_CMD_VOLATILE_SR_WRITE_ENABLE         0x50

//3. Nhóm lệnh nhận diện ID (Device Identification)
#define W25Q32_CMD_READ_JEDEC_ID                    0x9F  /* 3 bytes: 0xEF, 0x40, 0x16 */
#define W25Q32_CMD_READ_MANUFACTURER_ID             0x90
#define W25Q32_CMD_READ_UNIQUE_ID                   0x4B
#define W25Q32_CMD_READ_SFDP                        0x5A

//4. Nhóm lệnh quản lý nguồn & Reset mềm
#define W25Q32_CMD_POWER_DOWN                       0xB9
#define W25Q32_CMD_RELEASE_POWER_DOWN_ID            0xAB
#define W25Q32_CMD_ENABLE_RESET                     0x66
#define W25Q32_CMD_RESET_DEVICE                     0x99
#define W25Q32_CMD_ERASE_PROGRAM_SUSPEND            0x75
#define W25Q32_CMD_ERASE_PROGRAM_RESUME             0x7A

//5. Nhóm lệnh mở rộng (Dual/Quad SPI)
#define W25Q32_CMD_FAST_READ_DUAL_OUTPUT            0x3B
#define W25Q32_CMD_FAST_READ_DUAL_IO                0xBB
#define W25Q32_CMD_FAST_READ_QUAD_OUTPUT            0x6B
#define W25Q32_CMD_FAST_READ_QUAD_IO                0xEB
#define W25Q32_CMD_QUAD_PAGE_PROGRAM                0x32
#define W25Q32_CMD_SET_BURST_WITH_WRAP              0x77
#define W25Q32_CMD_READ_MFR_DEVICE_ID_DUAL_IO       0x92
#define W25Q32_CMD_READ_MFR_DEVICE_ID_QUAD_IO       0x94

//6. Nhóm lệnh bảo mật & Khóa ô nhớ (Security & Lock)
#define W25Q32_CMD_ERASE_SECURITY_REG               0x44
#define W25Q32_CMD_PROGRAM_SECURITY_REG             0x42
#define W25Q32_CMD_READ_SECURITY_REG                0x48
#define W25Q32_CMD_INDIVIDUAL_BLOCK_LOCK            0x36
#define W25Q32_CMD_INDIVIDUAL_BLOCK_UNLOCK          0x39
#define W25Q32_CMD_READ_BLOCK_LOCK                  0x3D
#define W25Q32_CMD_GLOBAL_BLOCK_LOCK                0x7E
#define W25Q32_CMD_GLOBAL_BLOCK_UNLOCK              0x98


//Struct

typedef struct
{
	SPI_HandleTypeDef 	*hspi;
	GPIO_TypeDef		*cs_port;
	uint16_t			cs_pin;
}	W25Q32_HandleTypedef;


extern uint8_t test_flash_raw_id[4];

//Functions Prototype

	/**
	 * @brief 				Hàm khởi tạo w25q32
	 * @param hspi			Địa chỉ của spi được sử dụng
	 * @param cs_port		Chân PORT của GPIO được config là chân CS
	 * @param cs_pin		Chân PIN của GPIO được config là chân CS
	 * @return 				HAL_StatusTypedef
	 */
	HAL_StatusTypeDef w25q32_init(SPI_HandleTypeDef *hspi, GPIO_TypeDef *cs_port, uint16_t cs_pin);

	/**
	 * @brief Hàm để cho phép ghi/xóa vào chip nhớ w25q32
	 */
	HAL_StatusTypeDef w25q32_write_enable(void);

	/**
	 *@brief 				Hàm cho biết w25q32 có đang sẵn sàng hoạt động không
	 *@param timeout_ms 	Thời gian đợi tối đa (ms)
	 */
	HAL_StatusTypeDef w25q32_wait_busy(uint32_t timeout_ms);

	/**
	 * @brief 					Hàm xóa dữ liệu trên sector
	 * @param sector_address 	Địa chỉ bắt đầu của sector muốn xóa
	 * @return 					HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef w25q32_erase_sector(uint32_t sector_address);

	/**
	 * @brief 				Hàm đọc giá trị được chứa trong w25q32
	 * @param address 		Địa chỉ bắt đầu nơi muốn đọc
	 * @param buffer		Địa chỉ bắt đầu của buffer dùng để lưu các dữ liệu đọc ra từ w25q32
	 * @param length		Độ dài của dữ liệu muốn đọc
	 * @return 				HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef w25q32_read(uint32_t address, uint8_t *buffer, uint32_t length);

	/**
	 * @brief				Hàm ghi giá trị vào từng page trong w25q32
	 * @param address 		Địa chỉ bắt đầu nơi muốn ghi
	 * @param buffer		Địa chỉ bắt đầu của buffer đang chứa dữ liệu muốn ghi vào w25q32
	 * @param length		Độ dài của dữ liệu muốn ghi
	 * @return 				HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef w25q32_write_page(uint32_t address, const uint8_t *buffer, uint32_t length);

	/**
	 * @brief				Hàm ghi giá trị vào w25q32
	 * @param address 		Địa chỉ bắt đầu nơi muốn ghi
	 * @param buffer		Địa chỉ bắt đầu của buffer đang chứa dữ liệu muốn ghi vào w25q32
	 * @param length		Độ dài của dữ liệu muốn ghi
	 * @return 				HAL_StatusTypeDef
	 */
	HAL_StatusTypeDef w25q32_write(uint32_t address, const uint8_t *buffer, uint32_t length);

#ifdef __cplusplus
}
#endif

#endif /* __W25Q32_H*/
