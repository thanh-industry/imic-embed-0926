#ifndef APP_LOG_H
#define APP_LOG_H

#include <stdint.h>

typedef enum {
    /* Log dinh ky (co the tat bang lenh "log off") */
    LOG_INFO,
    LOG_ERROR,
    LOG_I2C_TEMP,    /* value = 0.1 C */
    LOG_I2C_PRESS,   /* value = Pa */
    LOG_SPI_ID,      /* value = Device ID */
    LOG_ADC_VOLT,    /* value = mV */
    LOG_ADC_SOUND,   /* value = mV (p-p) */
    /* Phan hoi lenh */
    LOG_RAW,         /* in nguyen chuoi text */
    LOG_ECHO,        /* value = ky tu go vao */
    LOG_FMT_VOLT,    /* text + "x.xx V" (value = mV) */
    LOG_FMT_TEMP,    /* text + "x.x C"  (value = 0.1 C) */
    LOG_FMT_MV,      /* text + "n mV" */
    LOG_FMT_PA       /* text + "n Pa" */
} LogType_t;

typedef struct {
    LogType_t   type;
    int32_t     value;
    const char *text;   /* chi dung chuoi hang (string literal) */
} LogMsg_t;

extern volatile uint8_t g_log_periodic;

void LOG_Post(LogType_t type, int32_t value, const char *text);      /* khong cho */
void LOG_PostWait(LogType_t type, int32_t value, const char *text);  /* cho toi 50 ms */
int  LOG_Format(const LogMsg_t *m, char *buf, int len);

#endif
