#include "led.h"

/**
 * @brief  Hàm lõi: Cập nhật trạng thái 3 chân LED cùng lúc.
 *         Sử dụng kỹ thuật "đảo ngược logic" cho LED Cathode Chung.
 */
static void LED_WritePins(uint8_t red, uint8_t green, uint8_t blue)
{
    uint16_t pins_to_set = 0;
    uint16_t pins_to_reset = 0;

    // LOGIC ĐẢO NGƯỢC: Bật (1) -> Kéo xuống 0V (RESET). Tắt (0) -> Kéo lên 3.3V (SET).
    // Chỉ có duy nhất một màu được kéo xuống 0V, các màu còn lại bị kéo lên 3.3V.
    if (red) pins_to_reset |= LED_RED_PIN; else pins_to_set |= LED_RED_PIN;
    if (green) pins_to_reset |= LED_GREEN_PIN; else pins_to_set |= LED_GREEN_PIN;
    if (blue) pins_to_reset |= LED_BLUE_PIN; else pins_to_set |= LED_BLUE_PIN;

    // Ghi 1 lần vào thanh ghi BSRR để các chân đổi trạng thái đồng thời, không bị nhấp nháy
    LED_PORT->BSRR = pins_to_set | (pins_to_reset << 16);
}

/**
 * @brief  Khởi tạo trạng thái ban đầu (Tắt hết).
 */
void LED_Init(void) {
    LED_WritePins(0, 0, 0);
}

/**
 * @brief  Tắt toàn bộ 3 màu.
 */
void LED_All_Off(void) {
    LED_WritePins(0, 0, 0);
}

/**
 * @brief  Bật Đỏ, tắt 2 màu còn lại.
 */
void LED_Red_On(void) {
    LED_WritePins(1, 0, 0);
}

/**
 * @brief  Bật Xanh lá, tắt 2 màu còn lại.
 */
void LED_Green_On(void) {
    LED_WritePins(0, 1, 0);
}

/**
 * @brief  Bật Xanh dương, tắt 2 màu còn lại.
 */
void LED_Blue_On(void) {
    LED_WritePins(0, 0, 1);
}

/**
 * @brief  Pha màu cơ bản.
 */
void LED_Set_Color(uint8_t red, uint8_t green, uint8_t blue) {
    LED_WritePins(red, green, blue);
}
