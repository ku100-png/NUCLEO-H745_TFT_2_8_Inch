/**
  ******************************************************************************
  * @file    SPI.h
  * @author  Ku100
  * @version 
  * @date    15-July-2025
  * @brief   Header for SPI.c file
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
#ifndef _USE_SPI_H_
#define _USE_SPI_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32h7xx_hal.h"



/* Exported types ------------------------------------------------------------*/
extern SPI_HandleTypeDef hspi1;
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern uint8_t spi_write_byte(SPI_HandleTypeDef *hspi, uint8_t chByte);




#endif

/*-------------------------------END OF FILE-------------------------------*/

