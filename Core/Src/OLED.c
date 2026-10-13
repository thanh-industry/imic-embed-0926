#include "OLED.h"
#include "string.h"

// Bo dem man hinh 128 * (64/8) = 1024 bytes
static uint8_t OLED_Buffer[OLED_WIDTH * OLED_HEIGHT / 8];

// Ham gui lenh command
static void OLED_WriteCommand(uint8_t byte)
{
	HAL_I2C_Mem_Write(&hi2c1, OLED_BASE_ADDR, OLED_CMD, 1, &byte, 1, 1000);
// ngoai vi I2C1, dia chi slave, gui command, gui 8 bit, truyen vao dia chi chua commandd, timeout 1s de truyen
}
void OLED_Clear()
{
	memset(OLED_Buffer,0,sizeof(OLED_Buffer));
}
void OLED_Update_Screen()
{
	for (uint8_t i = 0; i < 8; i++)
	{
		OLED_WriteCommand(0xB0 + i); // Di chuyển con trỏ ghi tới Page i
		OLED_WriteCommand(0x00);     // Reset cột thấp về 0
		OLED_WriteCommand(0x10);     // Reset cột cao về 0

		// Gửi 128 bytes dữ liệu tương ứng của Page đó qua I2C
		HAL_I2C_Mem_Write(&hi2c1, OLED_BASE_ADDR, OLED_DATA,
				1, &OLED_Buffer[OLED_WIDTH * i], OLED_WIDTH,
				HAL_MAX_DELAY);
		//su dung ngoai vi I2C1, dia chi cua man OLED, cho OLED biet la truyen data, moi khung truyen
		// thanh ghi trong OLED la 8 bit, truyen tu PAGE thu i, tong la 128 bytes, HAL_MAX_DELAY khong gioi han thoi gian truyen
	}

}
void OLED_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    // Kiem tra neu toa do vuot qua pham vi hien thi thi bo qua
    if (x >= OLED_WIDTH || y >= OLED_HEIGHT) return;

    // Tinh toan vi tri byte va bit trong mang OLED_Buffer
    if (color) {
        OLED_Buffer[x + (y / 8) * OLED_WIDTH] |= (1 << (y % 8));
    } else {
        OLED_Buffer[x + (y / 8) * OLED_WIDTH] &= ~(1 << (y % 8));
    }
}
void OLED_Init()
{
	HAL_Delay(100); // Chờ nguồn OLED ổn định sau khi cấp điện

	OLED_WriteCommand(0xAE); // 1. Tắt màn hình (Display OFF)

	OLED_WriteCommand(0x20); // 2. Chọn chế độ đánh địa chỉ bộ nhớ (Memory Addressing Mode)
	OLED_WriteCommand(0x02); //    -> Set Page Addressing Mode

	OLED_WriteCommand(0xA1); // 3. Quét cột Segment ngược lại (Segment Re-map: SEG127 -> SEG0)
	OLED_WriteCommand(0xC8); // 4. Quét hàng COM ngược lại (COM Scan Direction: COM63 -> COM0)
							 // (A1 + C8 giúp hiển thị đúng chiều, không bị lộn ngược)

	OLED_WriteCommand(0xDA); // 5. Cấu hình phần cứng chân COM Pins
	OLED_WriteCommand(0x12); //    -> Alternative COM Pin Configuration (Chuẩn cho màn 128x64)[cite: 4]

	OLED_WriteCommand(0x8D); // 6. Bật mạch nhân áp nội (Charge Pump Setting)[cite: 4]
	OLED_WriteCommand(0x14); //    -> Enable Charge Pump (BẮT BUỘC để OLED có áp phát sáng)[cite: 4]

	OLED_WriteCommand(0xA6); // 7. Hiển thị bình thường (Normal Display: 1 = Sáng, 0 = Tắt)[cite: 4]
	OLED_WriteCommand(0xA4); // 8. Xuất nội dung theo bộ đệm GDDRAM (Entire Display ON Follow RAM)[cite: 4]

	OLED_WriteCommand(0xAF); // 9. Bật màn hình (Display ON)[cite: 4]

	// Xóa bộ đệm và cập nhật ngay để tránh màn hình bị sọc rác khi khởi động
	OLED_Clear();
	OLED_Update_Screen();
}

void OLED_DrawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t color) {
    if (w <= 0 || h <= 0) return;

    // 1. Vẽ 2 cạnh ngang (Cạnh trên & Cạnh dưới)
    for (int16_t i = x; i < x + w; i++) {
        OLED_DrawPixel(i, y, color);         // Cạnh trên
        OLED_DrawPixel(i, y + h - 1, color); // Cạnh dưới
    }

    // 2. Vẽ 2 cạnh dọc (Cạnh trái & Cạnh phải)
    for (int16_t j = y; j < y + h; j++) {
        OLED_DrawPixel(x, j, color);         // Cạnh trái
        OLED_DrawPixel(x + w - 1, j, color); // Cạnh phải
    }
}

