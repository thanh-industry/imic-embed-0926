//Include
#include "rtc_time.h"
#include <stdio.h>

//Value
static RTC_HandleTypeDef *p_hrtc = NULL; //Phải dúng một con trỏ trỏ tới luồng rtc, vì trong hàm main đã có khởi tạo hrtc rồi

//Functions

HAL_StatusTypeDef rtc_time_init(RTC_HandleTypeDef *hrtc)
{
	if(hrtc == NULL)
	{
		return HAL_ERROR;
	}
	p_hrtc = hrtc;
	return HAL_OK;
}

HAL_StatusTypeDef rtc_time_get_val(RTC_DateTime_t *dt)
{
	if(p_hrtc == NULL || dt == NULL)
	{
		return HAL_ERROR;
	}

	HAL_StatusTypeDef status;
	RTC_TimeTypeDef sTime = {0};
	RTC_DateTypeDef sDate = {0};

	status = HAL_RTC_GetTime(p_hrtc, &sTime, RTC_FORMAT_BIN);
	if(status != HAL_OK)
	{
		return HAL_ERROR;
	}

	status = HAL_RTC_GetDate(p_hrtc, &sDate, RTC_FORMAT_BIN);
	if(status != HAL_OK)
	{
		return HAL_ERROR;
	}

	dt->hours = sTime.Hours;
	dt->minutes = sTime.Minutes;
	dt->seconds = sTime.Seconds;

	dt->date = sDate.Date;
	dt->month = sDate.Month;
	dt->year = 2000 + sDate.Year;

	return HAL_OK;
}

void rtc_time_get_string(char *buffer)
{
	if(buffer == NULL)
	{
		return;
	}

	RTC_DateTime_t dt;

	if(rtc_time_get_val(&dt) == HAL_OK)
	{
		snprintf(buffer,RTC_STRING_BUFFER_SIZE, "%04d-%02d-%02d %02d:%02d:%02d", dt.year, dt.month, dt.date, dt.hours, dt.minutes, dt.seconds);
	}
	else
	{
		snprintf(buffer, RTC_STRING_BUFFER_SIZE, "RTC ERROR");
	}
}
