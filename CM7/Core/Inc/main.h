/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2025 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
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
#include "stm32h7xx_hal.h"
#include "PF_Config.h"
#include "Fonts.h"
#include "LCD.h"
#include "SPI.h"
#include "Touch.h"
#include "XPT2046.h"


/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
typedef struct
{
  void   (*DemoFunc)(void);
  uint8_t DemoName[30];
  uint32_t DemoIndex;
}BSP_DemoTypedef;
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
uint8_t CheckForUserInput(void);

void Demo_DrawGraphic(void);
void Demo_ShowImages(void);
void Demo_Touchscreen(void);
void Demo_SDwriteFile(void);
void Demo_ClimatParam(void);
/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define STLINK_RX_Pin GPIO_PIN_8
#define STLINK_RX_GPIO_Port GPIOD
#define STLINK_TX_Pin GPIO_PIN_9
#define STLINK_TX_GPIO_Port GPIOD
/* USER CODE BEGIN Private defines */
#define SD_SPI_HANDLE hspi1

#define SET_SPI_BAUDRATEPRESCALER_128		WRITE_REG(hspi1.Instance->CFG1, (SPI_BAUDRATEPRESCALER_128 | (hspi1.Instance->CFG1&0x0FFFFFFF)))
#define SET_SPI_BAUDRATEPRESCALER_32		WRITE_REG(hspi1.Instance->CFG1, (SPI_BAUDRATEPRESCALER_32 | (hspi1.Instance->CFG1&0x0FFFFFFF)))
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
