/*

  This is a library for the BH1750FVI Digital Light Sensor breakout board.

  The BH1750 board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The BH1750 library doesn't do this automatically.

  Datasheet:
  http://www.elechouse.com/elechouse/images/product/Digital%20light%20Sensor/bh1750fvi-e.pdf

  Written by Kulikov Vladimir, January, 2026.

*/

#ifndef BH1750_h
#define BH1750_h

#include "stm32h7xx_hal.h"

// Uncomment, to enable debug messages
// #define BH1750_DEBUG

// I2C Address BH1750
#define BH1750_I2CADDR		0x46	// 0b  0100 0110 (учитываетс€ и бит, отвечающий за R/W. Ѕез него 7-битный адресс 0x23)

// Commands for BH1750
#define BH1750_POWER_DOWN										0x00	// No active state
#define BH1750_POWER_ON											0x01	// Waiting for measurement command
#define BH1750_RESET												0x07	// Reset data register value - not accepted in POWER_DOWN mode
#define BH1750_CONTIN_HR_MODE								0x10
#define BH1750_CONTIN_HR_MODE2							0x11
#define BH1750_CONTIN_LR_MODE								0x13
#define BH1750_ONE_TIME_HR_MODE							0x20
#define BH1750_ONE_TIME_HR_MODE2						0x21
#define BH1750_ONE_TIME_LR_MODE							0x23

// Default MTreg value
#define BH1750_DEFAULT_MTREG 	69
#define BH1750_MTREG_MIN 			31
#define BH1750_MTREG_MAX 			254

// Prototype functions
uint8_t begin(uint8_t mode, uint8_t addr, I2C_HandleTypeDef *hi2c);
uint8_t configure(uint8_t mode);
uint8_t setMTreg(uint8_t MTreg);
uint8_t measurementReady(uint8_t maxWait);
uint8_t power_down(void);
uint8_t power_on(void);
float readLightLevel(void);


//	—труктура дл€ работы с I2C
//
//	ƒл€ каждого устройства, сид€щего на шине
// 	необходимо создать структуру данного типа
//
//	AdrSlave - адрес подчиненного устройства
//	*AdrDataTx - указатель на массив данных дл€ отправки.
//	DataLenTx - длина массива данных дл€ отправки.
//	*AdrDataRx - указатель на массив данных дл€ приема.
//	DataLenRx - длина массива данных дл€ приема.
//
//typedef struct
//{
//	uint8_t AdrSlave;	
//	uint8_t *AdrDataTx;
//	uint16_t DataLenTx;
//	uint8_t *AdrDataRx;
//	uint16_t DataLenRx;
//	
//}BH1750_I2CStruct;


#endif


