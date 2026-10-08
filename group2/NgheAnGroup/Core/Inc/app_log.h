/*
 * app_log.h
 *
 *  Created on: Oct 8, 2026
 *      Author: HACOM VINH
 */

#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdint.h>

typedef enum {
    LOG_INFO,       /* text */
    LOG_ERROR,      /* text */
    LOG_I2C_TEMP,   /* value = nhiet do, don vi 0.1 C */
    LOG_I2C_PRESS,  /* value = ap suat, don vi Pa */
    LOG_SPI_ID,     /* value = Device ID */
    LOG_ADC_VOLT,    /* value = dien ap, don vi mV */
	LOG_ADC_SOUND   /* value = bien do dinh-dinh, mV */
} LogType_t;

typedef struct {
    LogType_t   type;
    int32_t     value;
    const char *text;   /* chi dung chuoi hang (string literal) */
} LogMsg_t;

void LOG_Post(LogType_t type, int32_t value, const char *text);
int  LOG_Format(const LogMsg_t *m, char *buf, int len);

#endif
