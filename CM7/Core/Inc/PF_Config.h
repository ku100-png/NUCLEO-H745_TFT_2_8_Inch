/**
  ******************************************************************************
  * @file    PF_Config.h
  * @author  Ku100
  * @version 
  * @date    14-July-2025
  * @brief   This file provides configurations for low layer hardware libraries
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, WAVESHARE SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _USE_PF_CONFIG_H_
#define _USE_PF_CONFIG_H_
//Macro Definition

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"
#include "SPI.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/


typedef enum
{
  BUTTON_USER = 0U,
  BUTTONn
}Button_TypeDef;

typedef enum
{
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
}ButtonMode_TypeDef;

/*----------------------------------------------------------------------------*/

/**
  * @brief  LCD Control Lines management
  */
#define LCD_BKL_GPIO   GPIOD
#define LCD_DC_GPIO    GPIOG
#define LCD_CS_GPIO    GPIOD


#define LCD_BKL_PIN    GPIO_PIN_15
#define LCD_DC_PIN     GPIO_PIN_12
#define LCD_CS_PIN     GPIO_PIN_14


#define __LCD_BKL_SET()    HAL_GPIO_WritePin(LCD_BKL_GPIO, LCD_BKL_PIN, GPIO_PIN_SET)
#define __LCD_BKL_CLR()    HAL_GPIO_WritePin(LCD_BKL_GPIO, LCD_BKL_PIN, GPIO_PIN_RESET)

#define __LCD_CS_SET()     HAL_GPIO_WritePin(LCD_CS_GPIO, LCD_CS_PIN, GPIO_PIN_SET)
#define __LCD_CS_CLR()     HAL_GPIO_WritePin(LCD_CS_GPIO, LCD_CS_PIN, GPIO_PIN_RESET)

#define __LCD_DC_SET()     HAL_GPIO_WritePin(LCD_DC_GPIO, LCD_DC_PIN, GPIO_PIN_SET)
#define __LCD_DC_CLR()     HAL_GPIO_WritePin(LCD_DC_GPIO, LCD_DC_PIN, GPIO_PIN_RESET)
/**
  * @}
  */
	
/*----------------------------------------------------------------------------*/

/**
  * @brief  SD Control Lines management
  */
#define SD_CS_GPIO_PORT        GPIOE
#define SD_CS_PIN              GPIO_PIN_11

#define __SD_CS_SET()          HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_SET)
#define __SD_CS_CLR()          HAL_GPIO_WritePin(SD_CS_GPIO_PORT, SD_CS_PIN, GPIO_PIN_RESET)

//#define __SD_WRITE_BYTE(__DATA)       HAL_SPI_Transmit(&hspi1,&__DATA,1,0xff)
/**
  * @}
  */
	
/*----------------------------------------------------------------------------*/

/**
  * @brief  Touch Control Lines management
  */
#define XPT2046_CS_PIN          GPIO_PIN_14
#define XPT2046_IRQ_PIN         GPIO_PIN_13

#define XPT2046_CS_GPIO         GPIOE
#define XPT2046_IRQ_GPIO        GPIOE

#define __XPT2046_CS_SET()      HAL_GPIO_WritePin(XPT2046_CS_GPIO, XPT2046_CS_PIN, GPIO_PIN_SET)
#define __XPT2046_CS_CLR()      HAL_GPIO_WritePin(XPT2046_CS_GPIO, XPT2046_CS_PIN, GPIO_PIN_RESET)

#define __XPT2046_IRQ_READ()    HAL_GPIO_ReadPin(XPT2046_IRQ_GPIO, XPT2046_IRQ_PIN)

/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/**
  * @brief  LEDs Control Lines management
  */
/*
* Led1 - Green
* Led2 - Yellow
* Led3 - Red
*/
#define LED1_GPIO			GPIOB
#define LED2_GPIO			GPIOE
#define LED3_GPIO			GPIOB

#define LED1_PIN			GPIO_PIN_0
#define LED2_PIN			GPIO_PIN_1
#define LED3_PIN			GPIO_PIN_14

/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/** @defgroup STM32H7XX_NUCLEO_LOW_LEVEL_BUTTON LOW LEVEL BUTTON
  * @{
  */
/* Button state */
#define BUTTON_RELEASED                    0U
#define BUTTON_PRESSED                     1U
#define BUTTONn                            1U

/**
 * @brief Key push-button
 */
#define BUTTON_USER_PIN                       GPIO_PIN_13
#define BUTTON_USER_GPIO_PORT                 GPIOC
#define BUTTON_USER_GPIO_CLK_ENABLE()         __HAL_RCC_GPIOC_CLK_ENABLE()
#define BUTTON_USER_GPIO_CLK_DISABLE()        __HAL_RCC_GPIOC_CLK_DISABLE()
#define BUTTON_USER_EXTI_IRQn                 EXTI15_10_IRQn
#define BUTTON_USER_EXTI_LINE                 EXTI_LINE_13

#define BSP_ERROR_NONE            	        0

/* IRQ priorities */
#define BSP_BUTTON_USER_IT_PRIORITY         15U
/**
  * @}
  */
/*----------------------------------------------------------------------------*/

/* Exported functions ------------------------------------------------------- */
extern void driver_init(void);

int32_t  BSP_PB_Init(Button_TypeDef Button, ButtonMode_TypeDef ButtonMode);
int32_t  BSP_PB_DeInit(Button_TypeDef Button);
int32_t  BSP_PB_GetState(Button_TypeDef Button);
void     BSP_PB_IRQHandler(Button_TypeDef Button);
void     BSP_PB_Callback(Button_TypeDef Button);

uint32_t DWT_Delay_Init(void);
void DWT_Delay_us(volatile uint32_t microseconds);

#endif

/*--------------------------------END OF FILE---------------------------------*/

