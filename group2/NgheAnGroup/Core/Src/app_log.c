/*
 * app_log.c
 *
 *  Created on: Oct 8, 2026
 *      Author: HACOM VINH
 */
#include "app_log.h"
#include "cmsis_os.h"
#include <stdio.h>

extern osMessageQueueId_t LogQueueHandle;

void LOG_Post(LogType_t type, int32_t value, const char *text)
{
    LogMsg_t m = { type, value, text };
    osMessageQueuePut(LogQueueHandle, &m, 0, 0);  /* day thi bo qua, khong chan task */
}

int LOG_Format(const LogMsg_t *m, char *buf, int len)
{
    switch (m->type) {
    case LOG_INFO:
        return snprintf(buf, len, "[INFO] %s\r\n", m->text);
    case LOG_ERROR:
        return snprintf(buf, len, "[ERROR] %s\r\n", m->text);
    case LOG_I2C_TEMP: {
        int32_t v = m->value;
        int32_t a = (v < 0) ? -v : v;
        return snprintf(buf, len, "[I2C] Temperature = %s%ld.%ld C\r\n",
                        (v < 0) ? "-" : "", (long)(a / 10), (long)(a % 10));
    }
    case LOG_I2C_PRESS:
        return snprintf(buf, len, "[I2C] Pressure = %ld Pa\r\n", (long)m->value);
    case LOG_SPI_ID:
        return snprintf(buf, len, "[SPI] Device ID = 0x%lX\r\n", (unsigned long)m->value);
    case LOG_ADC_VOLT:
        return snprintf(buf, len, "[ADC] Voltage = %ld.%02ld V\r\n",
                        (long)(m->value / 1000), (long)((m->value % 1000) / 10));
    case LOG_ADC_SOUND:
        return snprintf(buf, len, "[ADC] Sound level = %ld mV (p-p)\r\n", (long)m->value);
    default:
        return 0;
    }
}

