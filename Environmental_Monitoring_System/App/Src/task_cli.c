#include "task_cli.h"
#include "app_main.h"
#include "cli_parser.h"
#include "main.h"
#include <string.h>
#include <stdio.h>

extern UART_HandleTypeDef huart2;

static uint8_t rx_byte = 0;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2)
  {
      /* Bật cố định đèn Xanh Dương (SET) thay vì Toggle để dễ quan sát */
      HAL_GPIO_WritePin(GPIOD, BLUE_LED_Pin, GPIO_PIN_SET);

      osMessageQueuePut(uartQueueHandle, &rx_byte, 0U, 0U);

      /* Bắt buộc mở khóa huart trước khi gọi lại Receive_IT để không bị HAL_BUSY */
      __HAL_UNLOCK(huart);
      HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  }
}

void HAL_UART_ErrorCallback(UART_HandleTypeDef *huart)
{
  if(huart->Instance == USART2)
  {
      /* Xóa cờ lỗi Overrun (ORE) khi gửi chuỗi nhanh và mở lại ngắt nhận */
      __HAL_UART_CLEAR_OREFLAG(huart);
      __HAL_UNLOCK(huart);
      HAL_UART_Receive_IT(&huart2, &rx_byte, 1);
  }
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

	__HAL_UART_CLEAR_OREFLAG(&huart2);
	__HAL_UNLOCK(&huart2);
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

    				// 1. Chiếm Mutex trước khi Parser vẽ lên màn hình OLED qua I2C2
					osMutexAcquire(oledMutexHandle, osWaitForever);

    				cli_parser_process(cmd_buffer);

    				// 2. Nhả Mutex ngay sau khi lệnh vẽ hoàn tất
					osMutexRelease(oledMutexHandle);

					oled_hold_seconds = 5; // Giữ màn hình kết quả CLI trong 5 giây

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
