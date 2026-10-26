#ifndef ADC_PIN_H
#define ADC_PIN_H
#include "main.h"
extern ADC_HandleTypeDef hadc1;

uint32_t Task_ADC();
float Bit_To_Vol();

#endif
