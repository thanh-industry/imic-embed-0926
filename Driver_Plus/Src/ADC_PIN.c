#include "ADC_PIN.h"

uint32_t Task_ADC()
{
	volatile uint32_t Vol_bit;
	HAL_ADC_Start(&hadc1);
	HAL_ADC_PollForConversion(&hadc1, 1000);
	Vol_bit = HAL_ADC_GetValue(&hadc1);
	HAL_ADC_Stop(&hadc1);

	return Vol_bit;
}
volatile float Vol;
float Bit_To_Vol()
{
	return Vol = (Task_ADC() * 3.3f) / 4095.0f;
}
