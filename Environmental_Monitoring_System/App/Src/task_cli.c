#include "task_cli.h"
#include "app_main.h"
#include "cli_parser.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

static uint8_t rx_byte = 0;

/**
  * @brief  Rx Transfer completed callbacks.
  * @param  huart  Pointer to a UART_HandleTypeDef structure that contains
  *                the configuration information for the specified UART module.
  * @retval None
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  /* Prevent unused argument(s) compilation warning */
  if(huart->Instance == USART2)
  {
	  osMessageQueuePut(uartQueueHandle, &rx_byte, 0U, 0U);
	  HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  }
  /* NOTE: This function should not be modified, when the callback is needed,
           the HAL_UART_RxCpltCallback could be implemented in the user file
   */
}

void StartCliTask(void *argument)
{
	uint8_t ch = 0;
	char cmd_buffer[CLI_BUFFER_SIZE];
	uint8_t cmd_index = 0;

	memset(cmd_buffer, 0, sizeof(cmd_buffer));

	const char *welcome = "\r\n------------------------------------------"
						  "\r\n* ENVIRONMENTAL MONITORING SYSTEM (CLI) *"
						  "\r\n* Go 'help' de xem danh sach lenh        *"
						  "\r\n------------------------------------------";

	HAL_UART_Transmit(&huart2, (uint8_t *)welcome, strlen(welcome), 100);
	HAL_UART_Transmit(&huart2, 	(uint8_t *)CLI_PROMPT, strlen(CLI_PROMPT), 100);

	HAL_UART_Receive_IT(&huart2, &rx_byte, 1);

    for (;;)
    {
    	if(osMessageQueueGet(uartQueueHandle, &ch, NULL, osWaitForever) == osOK)
    	{
    		//Trường hợp 1: Nhấn Enter (\r hoặc \n) -> Thực thi lệnh
    		if(ch == '\r' || ch == '\n')
    		{
    			HAL_UART_Transmit(&huart2, (uint8_t *)"\r\n", 2, 10);

    			if(cmd_index > 0)
    			{
    				cmd_buffer[cmd_index] = '\0';
    				cli_parser_process(cmd_buffer);

    				cmd_index  = 0;
    				memset(cmd_buffer, 0, sizeof(cmd_buffer));
    			}

    			HAL_UART_Transmit(&huart2, (uint8_t *)CLI_PROMPT, strlen(CLI_PROMPT), 10);

    		}
    		//Trường hợp 2: Nhấn phím Backspace / Delete -> Xóa ký tự
    		else if(ch == '\b' || ch == 0x7F)
    		{
    			if(cmd_index > 0)
    			{
    				cmd_index--;
    				cmd_buffer[cmd_index] = '\0';
    				HAL_UART_Transmit(&huart2, (uint8_t *)"\b \b", 3, 10);
    			}
    		}
    		//Trường hợp 3: Ký tự văn bản thông thường -> Gom vào bộ đệm
    		else
    		{
    			if (cmd_index < (CLI_BUFFER_SIZE - 1))
				{
					cmd_buffer[cmd_index++] = (char)ch;

					HAL_UART_Transmit(&huart2, &ch, 1, 10);
				}
				else
				{
					const char *warn = "\r\n[Warning] Bo dem lenh day!\r\n";
					HAL_UART_Transmit(&huart2, (uint8_t *)warn, strlen(warn), 20);
					cmd_index = 0;
					memset(cmd_buffer, 0, sizeof(cmd_buffer));
					HAL_UART_Transmit(&huart2, (uint8_t *)CLI_PROMPT, strlen(CLI_PROMPT), 10);
				}
    		}
    	}
    }
}
