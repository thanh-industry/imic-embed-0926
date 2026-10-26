/* USER CODE BEGIN Header */
// USART2: PA2->TX, PA3->RX
// I2C1: PB7->SDA, PB6->SCL
// SPI1: PA4->CS, PA5->SCK, PA6->MISO, PA7->MOSI
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "LCD.h"
#include "BMP280.h"
#include "ADC_PIN.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef enum
{
	BMP280,
	Power
}NameSensor_t;

typedef struct
{
	NameSensor_t name;
	double Tem;
	double Press;
	float Vol_Pin;
}SensorData_t;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
ADC_HandleTypeDef hadc1;

I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart2;

/* Definitions for Task_UART2 */
osThreadId_t Task_UART2Handle;
const osThreadAttr_t Task_UART2_attributes = {
  .name = "Task_UART2",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for Task_SPI */
osThreadId_t Task_SPIHandle;
const osThreadAttr_t Task_SPI_attributes = {
  .name = "Task_SPI",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal3,
};
/* Definitions for Task_I2C */
osThreadId_t Task_I2CHandle;
const osThreadAttr_t Task_I2C_attributes = {
  .name = "Task_I2C",
  .stack_size = 256 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal2,
};
/* Definitions for Task_ADC */
osThreadId_t Task_ADCHandle;
const osThreadAttr_t Task_ADC_attributes = {
  .name = "Task_ADC",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityBelowNormal1,
};
/* Definitions for myQueue01 */
osMessageQueueId_t myQueue01Handle;
const osMessageQueueAttr_t myQueue01_attributes = {
  .name = "myQueue01"
};
/* Definitions for myMutex01 */
osMutexId_t myMutex01Handle;
const osMutexAttr_t myMutex01_attributes = {
  .name = "myMutex01"
};
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART2_UART_Init(void);
static void MX_ADC1_Init(void);
void StartTask_UART(void *argument);
void StartTask_SPI(void *argument);
void StartTask_I2C(void *argument);
void StartTask_ADC(void *argument);

/* USER CODE BEGIN PFP */
static char buff[128];
void mPrint(const char *str, ...)
{
	osMutexAcquire(myMutex01Handle, osWaitForever);
	va_list ptr;
	va_start(ptr, str);
	vsnprintf(buff, sizeof(buff), str, ptr);
	va_end(ptr);
	HAL_UART_Transmit(&huart2, (uint8_t*) buff, strlen(buff), 100);
	memset(buff, 0, sizeof(buff));
	osMutexRelease(myMutex01Handle);
}
uint8_t CMD;
char CMD_Buffer[10];
int CMD_Index = 0;
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart -> Instance == USART2)
	{
		// khoi tao co trang thai chuac co bien nao duoc danh thuc
		BaseType_t xHigherPriorityTaskWoken = pdFALSE;

		if (CMD == '\n')
		{
			CMD_Index = 0;
			// danh thuc task UART de su li chuoi vua nhan
			vTaskNotifyGiveFromISR(Task_UART2Handle, &xHigherPriorityTaskWoken );
		}
		else
		{

			CMD_Buffer[CMD_Index ++] = CMD;
		}

		if(CMD_Index > sizeof(CMD_Buffer))
		{
			mPrint("\nNhap lenh command sai! Vui long nhap lai.");
			CMD_Index = 0;
			memset(CMD_Buffer,0, sizeof(CMD_Buffer));
		}

		HAL_UART_Receive_IT(&huart2, &CMD, 1);
		// chuyen sang task UART day de xu ly ngat
		portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
	}
}
static volatile uint8_t flag_button = 0;
static volatile uint32_t last_time = 0;
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if(GPIO_Pin == GPIO_PIN_0)
	{
		uint32_t now = HAL_GetTick();
		if((now - last_time) > 300)
		{
			last_time = now;
			flag_button ^= 1;
		}
	}
}
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{

  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_SPI1_Init();
  MX_USART2_UART_Init();
  MX_ADC1_Init();
  /* USER CODE BEGIN 2 */
  LCD_Init();
  BMP280_Init();
  /* USER CODE END 2 */

  /* Init scheduler */
  osKernelInitialize();
  /* Create the mutex(es) */
  /* creation of myMutex01 */
  myMutex01Handle = osMutexNew(&myMutex01_attributes);

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of myQueue01 */
  myQueue01Handle = osMessageQueueNew (16, sizeof(SensorData_t), &myQueue01_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of Task_UART2 */
  Task_UART2Handle = osThreadNew(StartTask_UART, NULL, &Task_UART2_attributes);

  /* creation of Task_SPI */
  Task_SPIHandle = osThreadNew(StartTask_SPI, NULL, &Task_SPI_attributes);

  /* creation of Task_I2C */
  Task_I2CHandle = osThreadNew(StartTask_I2C, NULL, &Task_I2C_attributes);

  /* creation of Task_ADC */
  Task_ADCHandle = osThreadNew(StartTask_ADC, NULL, &Task_ADC_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

  /* Start scheduler */
  osKernelStart();

  /* We should never get here as control is now taken by the scheduler */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_3) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Configure the global features of the ADC (Clock, Resolution, Data Alignment and number of conversion)
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
  hadc1.Init.Resolution = ADC_RESOLUTION_12B;
  hadc1.Init.ScanConvMode = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.DataAlign = ADC_DATAALIGN_RIGHT;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DMAContinuousRequests = DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
  */
  sConfig.Channel = ADC_CHANNEL_1;
  sConfig.Rank = 1;
  sConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_16;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART2_UART_Init(void)
{

  /* USER CODE BEGIN USART2_Init 0 */

  /* USER CODE END USART2_Init 0 */

  /* USER CODE BEGIN USART2_Init 1 */

  /* USER CODE END USART2_Init 1 */
  huart2.Instance = USART2;
  huart2.Init.BaudRate = 115200;
  huart2.Init.WordLength = UART_WORDLENGTH_8B;
  huart2.Init.StopBits = UART_STOPBITS_1;
  huart2.Init.Parity = UART_PARITY_NONE;
  huart2.Init.Mode = UART_MODE_TX_RX;
  huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart2.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART2_Init 2 */

  /* USER CODE END USART2_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
  /* USER CODE BEGIN MX_GPIO_Init_1 */

  /* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, GPIO_PIN_RESET);

  /*Configure GPIO pin : PA0 */
  GPIO_InitStruct.Pin = GPIO_PIN_0;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pin : PA4 */
  GPIO_InitStruct.Pin = GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI0_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI0_IRQn);

  /* USER CODE BEGIN MX_GPIO_Init_2 */

  /* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartTask_UART */
/**
  * @brief  Function implementing the Task_UART1 thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartTask_UART */
void StartTask_UART(void *argument)
{
  /* USER CODE BEGIN 5 */
  /* Infinite loop */
  mPrint("TASK UART: RUNNING\r\n");
  for(;;)
  {

    if (ulTaskNotifyTake(pdTRUE, portMAX_DELAY) > 0)
    {
    	mPrint("Nhan duoc command: %s\n", CMD_Buffer);
    	if (strcmp(CMD_Buffer,"help") == 0)
    	{
    		mPrint("\nstatus\nsensor\nvoltage\nversion\n");
    		memset(CMD_Buffer, 0, sizeof(CMD_Buffer));
    	}
    	else if (strcmp(CMD_Buffer,"status") == 0)
    	{
    		// hien trang thai cac sensor
    	}
    	else if (strcmp(CMD_Buffer,"voltage") == 0)
    	{
    		// hien dien ap pin
    	}
    	else if (strcmp(CMD_Buffer,"version") == 0)
    	{
    		mPrint("Version of Thanh and Hai.\n");
    		memset(CMD_Buffer, 0, sizeof(CMD_Buffer));
    	}
    	else
    	{
    		mPrint("Command Not Found!!!\n");
    		memset(CMD_Buffer, 0, sizeof(CMD_Buffer));
    	}
    }
  }
  osDelay(1000);
  /* USER CODE END 5 */
}

/* USER CODE BEGIN Header_StartTask_SPI */
/**

* @brief Function implementing the Task_SPI thread.
* @param argument: Not used
* @retval None
*/
double test_T, test_P;
/* USER CODE END Header_StartTask_SPI */
void StartTask_SPI(void *argument)
{
  /* USER CODE BEGIN StartTask_SPI */
  /* Infinite loop */
  mPrint("TASK Sensor SPI: RUNNING\n");
  static SensorData_t data;
  data.name = BMP280;
  for(;;)
  {
	uint32_t adc_T = BMP280_ADC_T();
	uint32_t adc_P = BMP280_ADC_P();
	double T = BMP280_compensate_T_double(adc_T);
	double P = BMP280_compensate_P_double(adc_P);
	test_T = T;
	test_P = P;
	data.Tem = T;
	data.Press = P;
	osMessageQueuePut(myQueue01Handle, &data, 0,100);
    osDelay(1000);
  }
  /* USER CODE END StartTask_SPI */
}

/* USER CODE BEGIN Header_StartTask_I2C */
/**
* @brief Function implementing the Task_I2C thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_I2C */
void StartTask_I2C(void *argument)
{
  /* USER CODE BEGIN StartTask_I2C */
  /* Infinite loop */
  mPrint("TASK LCD I2C: RUNNING\r\n");
  char LCD_buff[20];
  SensorData_t rx_data;
  for(;;)
  {
	if (osMessageQueueGet(myQueue01Handle, &rx_data, 0, osWaitForever) == osOK)
	{
			if (flag_button == 0 && rx_data.name == Power)
			{
				LCD_Row_1;
				snprintf(LCD_buff, sizeof(LCD_buff), "Power :  %.2f  V",rx_data.Vol_Pin);
				Send_String(LCD_buff);
				memset(LCD_buff, 0, sizeof(LCD_buff));
				LCD_Row_2;
			    snprintf(LCD_buff,sizeof(LCD_buff) ,"                ");
				Send_String(LCD_buff);
			    memset(LCD_buff,0,sizeof(LCD_buff));
			}
			else if (flag_button == 1 && rx_data.name == BMP280)
			{
			   LCD_Row_1;
			   snprintf(LCD_buff,sizeof(LCD_buff) ,"Tem:  %.2f   \xDF""C", rx_data.Tem);
			   Send_String(LCD_buff);
			   memset(LCD_buff,0,sizeof(LCD_buff));
			   LCD_Row_2;
			   snprintf(LCD_buff,sizeof(LCD_buff) ,"Press: %.2f  bar", rx_data.Press);
			   Send_String(LCD_buff);
			   memset(LCD_buff,0,sizeof(LCD_buff));
			}
	}
  }
  /* USER CODE END StartTask_I2C */
}

/* USER CODE BEGIN Header_StartTask_ADC */
/**
* @brief Function implementing the Task_ADC thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartTask_ADC */
void StartTask_ADC(void *argument)
{
  /* USER CODE BEGIN StartTask_ADC */
  mPrint("TASK ADC PIN: RUNNING\n");
  /* Infinite loop */
  static SensorData_t data;
  data.name = Power;
  for(;;)
  {
	data.Vol_Pin = Bit_To_Vol();
	osMessageQueuePut(myQueue01Handle, &data, 0, 100);
    osDelay(3000);
  }
  /* USER CODE END StartTask_ADC */
}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4)
  {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}
#ifdef USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
