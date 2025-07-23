/**
  ******************************************************************************
  * @file    SPI.c 
  * @author  Ku100
  * @version 
  * @date    15-July-2025
  * @brief   This file provides a set of functions to manage the communication between SPI peripherals.
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



/* Includes ------------------------------------------------------------------*/
#include "PF_Config.h"
#include "SPI.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
uint8_t spi_write_byte(SPI_HandleTypeDef *hspi, uint8_t chByte);


/* Private functions ---------------------------------------------------------*/

/**
  * @brief Read or write an byte from or to SPI bus.
  * @param  tSPIx: where x can be 1 , 2 or 3 to select the SPI pripheral.
  * @retval chByte: Receive an byte from SPI bus
  */
uint8_t spi_write_byte(SPI_HandleTypeDef *hspi, uint8_t chByte)
{		
	int32_t ret = 0;
	uint8_t Lenght = 1;
	uint8_t Timeout = 200;
//	uint8_t *pRxData;
//	uint8_t RxData;
	
	
	if(HAL_SPI_Transmit(hspi, &chByte, Lenght, Timeout) != HAL_OK)
	{
		ret = 1;
	}
	return ret;
	
//	if(HAL_SPI_TransmitReceive(hspi, &chByte, pRxData, Lenght, Timeout) != HAL_OK)
//	{
//		ret = 1;
//	}
//	RxData = *pRxData;
//	
//	return RxData;
}



/*-------------------------------END OF FILE-------------------------------*/

