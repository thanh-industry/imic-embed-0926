#include "app_log.h"
#include "cmsis_os.h"
#include <stdio.h>

extern osMessageQueueId_t LogQueueHandle;

volatile uint8_t g_log_periodic = 1;

void LOG_Post(LogType_t type, int32_t value, const char *text)
{
    if (!g_log_periodic && type >= LOG_I2C_TEMP && type <= LOG_ADC_SOUND) {
        return;
    }
    LogMsg_t m = { type, value, text };
    osMessageQueuePut(LogQueueHandle, &m, 0, 0);
}

void LOG_PostWait(LogType_t type, int32_t value, const char *text)
{
    LogMsg_t m = { type, value, text };
    osMessageQueuePut(LogQueueHandle, &m, 0, 50);
}

int LOG_Format(const LogMsg_t *m, char *buf, int len)
{
    int32_t v = m->value;
    int32_t a = (v < 0) ? -v : v;

    switch (m->type) {
    case LOG_INFO:
        return snprintf(buf, len, "[INFO] %s\r\n", m->text);
    case LOG_ERROR:
        return snprintf(buf, len, "[ERROR] %s\r\n", m->text);
    case LOG_I2C_TEMP:
        return snprintf(buf, len, "[I2C] Temperature = %s%ld.%ld C\r\n",
                        (v < 0) ? "-" : "", (long)(a / 10), (long)(a % 10));
    case LOG_I2C_PRESS:
        return snprintf(buf, len, "[I2C] Pressure = %ld Pa\r\n", (long)v);
    case LOG_SPI_ID:
        return snprintf(buf, len, "[SPI] Device ID = 0x%lX\r\n", (unsigned long)v);
    case LOG_ADC_VOLT:
        return snprintf(buf, len, "[ADC] Voltage = %ld.%02ld V\r\n",
                        (long)(v / 1000), (long)((v % 1000) / 10));
    case LOG_ADC_SOUND:
        return snprintf(buf, len, "[ADC] Sound level = %ld mV (p-p)\r\n", (long)v);

    case LOG_RAW:
        return snprintf(buf, len, "%s", m->text);
    case LOG_ECHO:
        if (v == '\r') return snprintf(buf, len, "\r\n");
        return snprintf(buf, len, "%c", (char)v);
    case LOG_FMT_VOLT:
        return snprintf(buf, len, "%s%ld.%02ld V\r\n", m->text,
                        (long)(v / 1000), (long)((v % 1000) / 10));
    case LOG_FMT_TEMP:
        return snprintf(buf, len, "%s%s%ld.%ld C\r\n", m->text,
                        (v < 0) ? "-" : "", (long)(a / 10), (long)(a % 10));
    case LOG_FMT_MV:
        return snprintf(buf, len, "%s%ld mV\r\n", m->text, (long)v);
    case LOG_FMT_PA:
        return snprintf(buf, len, "%s%ld Pa\r\n", m->text, (long)v);
    default:
        return 0;
    }
}
