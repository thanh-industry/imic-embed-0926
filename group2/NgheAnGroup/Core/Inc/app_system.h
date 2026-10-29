/*
 * app_system.h
 *
 *  Created on: Oct 8, 2026
 *      Author: HACOM VINH
 */

#ifndef APP_SYSTEM_H
#define APP_SYSTEM_H

#include <stdint.h>

typedef struct {
    int32_t  temp_x10;   /* 0.1 C */
    int32_t  press_pa;   /* Pa */
    uint8_t  i2c_ok;
    uint8_t  spi_ok;
    uint32_t spi_id;
    int32_t  adc_mv;     /* mV, gia tri trung binh */
    int32_t  sound_pp_mv;/* mV, bien do dinh-dinh */
} SystemData_t;

extern SystemData_t g_sys;

#endif
