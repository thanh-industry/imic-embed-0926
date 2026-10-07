#include "stm32g0xx_hal.h"

extern UART_HandleTypeDef huart1;

void ble_tx(char * data, int length)
{
    HAL_UART_Transmit(&huart1, data, length);
}

void ble_rx(char *data, int *received_length)
{
   // HAL_UART_Receive(&huart1, data, &length);
}