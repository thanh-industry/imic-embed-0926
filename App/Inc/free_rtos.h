#ifndef __FREE_RTOS_H
#define __FREE_RTOS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"
#include "cmsis_os.h"
#include "adc.h"
#include "i2c_bmp280.h"
#include "i2c_oled_lcd.h"
#include "spi_w25qxx.h"
#include "led.h"
#include "logging.h"

void MX_FREERTOS_Init(void);
void UART_RxByteFromISR(uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* __FREE_RTOS_H */
