#ifndef RTC_TIME_H
#define RTC_TIME_H

#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"

//Define
#define RTC_STRING_BUFFER_SIZE    25

//Struct
typedef struct
{
	uint8_t hours;
	uint8_t minutes;
	uint8_t seconds;
	uint8_t date;
	uint8_t month;
	uint16_t year;
} RTC_DateTime_t;

//Functions Prototype

	/**
	* @brief	Hàm khởi tạo RTC
	* @return 	HAL_StatusTypeDef
	*/
	HAL_StatusTypeDef rtc_time_init(RTC_HandleTypeDef *hrtc);

	/**
	 * @brief 		Hàm đọc thời gian từ bộ RTC
	 * @param dt	Con trỏ trỏ tới struct chứa dữ liệu ngày/tháng/năm, giờ/phút/giây
	 * @return		HAL_StatusTypeDef: Trạng thái đọc (HAL_OK / HAL_ERROR)
	 */
	HAL_StatusTypeDef rtc_time_get_val(RTC_DateTime_t *dt);

	/**
	 * @brief			Chuyển đổi thời gian thành chuỗi định dạng (YYYY-MM-DD HH:MM:SS)
	 * @param buffer	Vùng nhớ lưu chuỗi ký tự (kích thước >= RTC_STRING_BUFFER_SIZE)
	 */
	void rtc_time_get_string(char *buffer);

#ifdef __cplusplus
}
#endif

#endif /* RTC_TIME_H */
