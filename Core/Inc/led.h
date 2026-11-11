/* led.h */
#ifndef __LED_H
#define __LED_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

/* ==========================================================
 * ĐỊNH NGHĨA PHẦN CỨNG (Dựa trên cấu hình CubeMX)
 * ========================================================== */
#define LED_PORT        GPIOD
#define LED_RED_PIN     GPIO_PIN_12
#define LED_GREEN_PIN   GPIO_PIN_13
#define LED_BLUE_PIN    GPIO_PIN_14

/* ==========================================================
 * CẤU HÌNH MỨC LOGIC (Tùy chỉnh theo mạch thực tế)
 * LED_ON: Mức điện áp để bật LED (SET = 3.3V, RESET = 0V)
 * LED_OFF: Mức điện áp để tắt LED
 * ========================================================== */
#define LED_ON   GPIO_PIN_SET
#define LED_OFF  GPIO_PIN_RESET

/* ==========================================================
 * KHAI BÁO NGUYÊN MẪU HÀM (PROTOTYPES)
 * ========================================================== */

/**
 * @brief  Khởi tạo trạng thái ban đầu cho các chân LED (Tắt hết).
 */
void LED_Init(void);

/**
 * @brief  Tắt toàn bộ 3 màu LED.
 */
void LED_All_Off(void);

/**
 * @brief  Bật màu Đỏ, tắt 2 màu còn lại.
 */
void LED_Red_On(void);

/**
 * @brief  Bật màu Xanh lá, tắt 2 màu còn lại.
 */
void LED_Green_On(void);


/**
 * @brief  Bật màu Xanh dương, tắt 2 màu còn lại.
 */
void LED_Blue_On(void);

/**
 * @brief  Pha màu cơ bản.
 * @param  red: 1 = Bật, 0 = Tắt
 * @param  green: 1 = Bật, 0 = Tắt
 * @param  blue: 1 = Bật, 0 = Tắt
 */
void LED_Set_Color(uint8_t red, uint8_t green, uint8_t blue);

#ifdef __cplusplus
}
#endif

#endif /* __LED_H */
