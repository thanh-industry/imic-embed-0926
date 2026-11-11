/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  ******************************************************************************
  */
/* USER CODE END Header */


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
// KHÔNG include các file driver ở đây để tránh vòng lặp include (Circular Dependency)
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */
/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/

/* USER CODE BEGIN Private defines */

/* ==========================================================
 * ĐỊNH NGHĨA CHÂN GPIO CHO CÁC NGOẠI VI
 * ========================================================== */

/* 1. Chip nhớ SPI (W25Qxx) - Chân CS */
#define W25Q_CS_PORT  GPIOC
#define W25Q_CS_PIN   GPIO_PIN_4

/* 2. LED RGB - Chân điều khiển */
#define LED_PORT      GPIOD
#define LED_RED_PIN   GPIO_PIN_12
#define LED_GREEN_PIN GPIO_PIN_13
#define LED_BLUE_PIN  GPIO_PIN_14

/* USER CODE END Private defines */

/* ==========================================================
 * KHAI BÁO CÁC HANDLE NGOẠI VI TOÀN CỤC (Dùng chung)
 * ========================================================== */
extern ADC_HandleTypeDef hadc1;
extern I2C_HandleTypeDef hi2c1;
extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern UART_HandleTypeDef huart2;
extern UART_HandleTypeDef huart3;

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
