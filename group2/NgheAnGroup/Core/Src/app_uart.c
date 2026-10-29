/*
 * app_uart.c
 *
 *  Created on: Oct 8, 2026
 *      Author: HACOM VINH
 */
#include "app_uart.h"
#include "app_log.h"
#include "main.h"
#include "usart.h"
#include "cmsis_os.h"

extern osMessageQueueId_t LogQueueHandle;
extern osMessageQueueId_t CmdRxQueueHandle;

static uint8_t rx_byte;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        osMessageQueuePut(CmdRxQueueHandle, &rx_byte, 0, 0);
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
    if (huart->Instance == USART2) {
        HAL_UART_Receive_IT(&huart2, &rx_byte, 1);  /* loi overrun/framing: bat nhan lai */
    }
}

void APP_UART_TaskLoop(void)
{
    LogMsg_t m;
    char line[80];

    HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
    LOG_Post(LOG_INFO, 0, "System started");

    for (;;) {
        if (osMessageQueueGet(LogQueueHandle, &m, NULL, osWaitForever) == osOK) {
            int n = LOG_Format(&m, line, sizeof(line));
            if (n > 0) {
                if (n >= (int)sizeof(line)) n = sizeof(line) - 1;
                HAL_UART_Transmit(&huart2, (uint8_t *)line, n, 100);
            }
        }
    }
}

