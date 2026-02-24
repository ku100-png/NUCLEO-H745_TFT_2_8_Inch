/*

  This is a library for the BH1750FVI Digital Light Sensor breakout board.

  The BH1750 board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The BH1750 library doesn't do this automatically.

  Written by Kulikov Vladimir, January, 2026.

*/

#include "BH1750.h"
#include "stm32h7xx_hal.h"
#include "string.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2c1;
extern UART_HandleTypeDef huart3;
extern uint16_t size;
extern uint8_t Data[256];


// Use Wiring's delay for compability with another platforms
#  define _delay_ms(ms) HAL_Delay(ms)

// Correction factor used to calculate lux. Typical value is 1.2 but can
// range from 0.96 to 1.44. See the data sheet (p.2, Measurement Accuracy)
// for more information.
const float BH1750_CONV_FACTOR = 1.2;

//BH1750_I2CStruct I2CStructBH1750; // Структура данных для работы с датчиком BH1750 через I2C (освещенность)

/**
 * Configure sensor
 * @param mode Measurement mode
 * @param addr Address of the sensor
 * @param i2c TwoWire instance connected to I2C bus
 */
uint8_t begin(uint8_t mode, uint8_t addr, I2C_HandleTypeDef *i2c) {

  // I2C is expected to be initialized outside this library
	// Use extern I2C_HandleTypeDef hi2c1

  // Configure sensor in specified mode and set default MTreg
  return (configure(mode) && setMTreg(BH1750_DEFAULT_MTREG));
}

/**
 * Configure BH1750 with specified mode
 * @param mode Measurement mode
 * @return 1 if mode successful set,
 * 				 0 if mode not changed or parameter out of range
 */
uint8_t configure(uint8_t mode){

  // default transmission result to a value out of normal range
  uint8_t ack = 4;
	uint8_t 	DataTransmitToBH1750[1];
	DataTransmitToBH1750[0] = mode;
	uint16_t Size = sizeof(DataTransmitToBH1750);
	uint32_t Timeout = 1000;

  // Check measurement mode is valid
  switch (mode) {

  case BH1750_CONTIN_HR_MODE:
  case BH1750_CONTIN_HR_MODE2:
  case BH1750_CONTIN_LR_MODE:
  case BH1750_ONE_TIME_HR_MODE:
  case BH1750_ONE_TIME_HR_MODE2:
  case BH1750_ONE_TIME_LR_MODE:
		

   // Send mode to sensor
	ack = HAL_I2C_Master_Transmit(&hi2c1, BH1750_I2CADDR, DataTransmitToBH1750, Size, Timeout);

	// Wait a few moments to wake up
	_delay_ms(10);
	break;

  default:
    // Invalid measurement mode
    //Serial.println(F("[BH1750] ERROR: Invalid mode"));
    break;
  }

  // Check result code
  switch (ack) {
	case 0:
		//size = sprintf((char *)Data, "BH1750: Status I2C - HAL_OK\n\r");	
		//HAL_UART_Transmit(&huart3, Data, size, 1000);
    return 1;
  case 1: // HAL_ERROR
    size = sprintf((char *)Data, "BH1750:configure error HAL_ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
    break;
  case 2: // HAL_BUSY
    size = sprintf((char *)Data, "BH1750: error HAL_BUSY\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
    break;
  case 3: // HAL_TIMEOUT
    size = sprintf((char *)Data, "BH1750: error HAL_TIMEOUT\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
    break;
  default:
		size = sprintf((char *)Data, "BH1750: undefined error\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
    break;
  }

  return 0;
}

/**
 * Configure BH1750 MTreg value
 * MT reg = Measurement Time register
 * @param MTreg a value between 31 and 254. Default: 69
 * @return 1 if MTReg successful set,
 * 				 0 if MTreg not changed or parameter out of range
 */
uint8_t setMTreg(uint8_t MTreg) {
  if (MTreg < BH1750_MTREG_MIN || MTreg > BH1750_MTREG_MAX) {
    //Serial.println(F("[BH1750] ERROR: MTreg out of range"));
    return 0;
  }
  uint8_t ack = 4;
  // Send MTreg and the current mode to the sensor
  //   High bit: 01000_MT[7,6,5]
  //    Low bit: 011_MT[4,3,2,1,0]
	uint8_t 	DataTransmitToBH1750[2];
	DataTransmitToBH1750[0] = (0b01000 << 3) | (MTreg >> 5);
	DataTransmitToBH1750[1] = (0b011 << 5) | (MTreg & 0b11111);
	//uint16_t Size = sizeof(DataTransmitToBH1750);
	uint32_t Timeout = 1000;
	
	ack = HAL_I2C_Master_Transmit(&hi2c1, BH1750_I2CADDR, &DataTransmitToBH1750[0], 1, Timeout);
	ack = HAL_I2C_Master_Transmit(&hi2c1, BH1750_I2CADDR, &DataTransmitToBH1750[1], 1, Timeout);

  // Wait a few moments to wake up
  _delay_ms(10);

  // Check result code
  switch (ack) {
		case 0:
			//size = sprintf((char *)Data, "BH1750: Status I2C - HAL_OK\n\r");	
			//HAL_UART_Transmit(&huart3, Data, size, 1000);
			return 1;
		case 1: // HAL_ERROR
			size = sprintf((char *)Data, "BH1750:setMTreg error HAL_ERROR\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 2: // HAL_BUSY
			size = sprintf((char *)Data, "BH1750: error HAL_BUSY\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 3: // HAL_TIMEOUT
			size = sprintf((char *)Data, "BH1750: error HAL_TIMEOUT\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		default:
			size = sprintf((char *)Data, "BH1750: undefined error\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		}

  return 0;
}

///**
// * Checks whether enough time has gone to read a new value
// * @param maxWait a boolean if to wait for typical or maximum delay
// * @return a boolean if a new measurement is possible
// *
// */
//bool BH1750::measurementReady(bool maxWait) {
//  unsigned long delaytime = 0;
//  switch (BH1750_MODE) {
//  case BH1750::CONTINUOUS_HIGH_RES_MODE:
//  case BH1750::CONTINUOUS_HIGH_RES_MODE_2:
//  case BH1750::ONE_TIME_HIGH_RES_MODE:
//  case BH1750::ONE_TIME_HIGH_RES_MODE_2:
//    maxWait ? delaytime = (180 * BH1750_MTreg / (byte)BH1750_DEFAULT_MTREG)
//            : delaytime = (120 * BH1750_MTreg / (byte)BH1750_DEFAULT_MTREG);
//    break;
//  case BH1750::CONTINUOUS_LOW_RES_MODE:
//  case BH1750::ONE_TIME_LOW_RES_MODE:
//    // Send mode to sensor
//    maxWait ? delaytime = (24 * BH1750_MTreg / (byte)BH1750_DEFAULT_MTREG)
//            : delaytime = (16 * BH1750_MTreg / (byte)BH1750_DEFAULT_MTREG);
//    break;
//  default:
//    break;
//  }
//  // Wait for new measurement to be possible.
//  // Measurements have a maximum measurement time and a typical measurement
//  // time. The maxWait argument determines which measurement wait time is
//  // used when a one-time mode is being used. The typical (shorter)
//  // measurement time is used by default and if maxWait is set to True then
//  // the maximum measurement time will be used. See data sheet pages 2, 5
//  // and 7 for more details.
//  unsigned long currentTimestamp = millis();
//  if (currentTimestamp - lastReadTimestamp >= delaytime) {
//    return true;
//  } else
//    return false;
//}

/**
 * Read light level from sensor
 * The return value range differs if the MTreg value is changed. The global
 * maximum value is noted in the square brackets.
 * @return Light level in lux (0.0 ~ 54612,5 [117758,203])
 * 	   -1 : no valid return value
 * 	   -2 : sensor not configured
 */
float readLightLevel(void) {
	uint8_t status;
//  if (BH1750_MODE == UNCONFIGURED) {
//    Serial.println(F("[BH1750] Device is not configured!"));
//    return -2.0;
//  }

  // Measurement result will be stored here
  float level = -1.0;
	
	uint8_t 	DataReceiveFromBH1750[]	= {0, 0};
	uint16_t Size = sizeof(DataReceiveFromBH1750);
	uint32_t Timeout = 1000;
	
	// Read two bytes from the sensor, which are low and high parts of the sensor value
	status = HAL_I2C_Master_Receive(&hi2c1, BH1750_I2CADDR, DataReceiveFromBH1750, Size, Timeout);
	
	if(status){
		switch (status)
		{
			// HAL_ERROR
			case 1:
				size = sprintf((char *)Data, "BH1750:readLightLevel error HAL_ERROR\n\r");	
				HAL_UART_Transmit(&huart3, Data, size, 1000);
				break;
			
			// HAL_BUSY
			case 2:
				size = sprintf((char *)Data, "BH1750: error HAL_BUSY\n\r");	
				HAL_UART_Transmit(&huart3, Data, size, 1000);
				break;
			
			// HAL_TIMEOUT
			case 3:
				size = sprintf((char *)Data, "BH1750: error HAL_TIMEOUT\n\r");	
				HAL_UART_Transmit(&huart3, Data, size, 1000);
				break;
			
			default:
				size = sprintf((char *)Data, "BH1750: undefined error\n\r");	
				HAL_UART_Transmit(&huart3, Data, size, 1000);
				break;
		}
	}
	
	unsigned int tmp = 0;
	tmp = DataReceiveFromBH1750[0];
	tmp <<= 8;
	tmp |= DataReceiveFromBH1750[1];
	level = tmp;

  if (level != -1.0) {
// Print raw value if debug enabled
#ifdef BH1750_DEBUG
    Serial.print(F("[BH1750] Raw value: "));
    Serial.println(level);
#endif

//    if (BH1750_MTreg != BH1750_DEFAULT_MTREG) {
//     level *= (float)((byte)BH1750_DEFAULT_MTREG / (float)BH1750_MTreg);
// Print MTreg factor if debug enabled
#ifdef BH1750_DEBUG
      Serial.print(F("[BH1750] MTreg factor: "));
      Serial.println(
      String((float)((byte)BH1750_DEFAULT_MTREG / (float)BH1750_MTreg)));
#endif
//    }
//    if (BH1750_MODE == BH1750::ONE_TIME_HIGH_RES_MODE_2 ||
//        BH1750_MODE == BH1750::CONTINUOUS_HIGH_RES_MODE_2) {
//      level /= 2;
//    }
    // Convert raw value to lux
    level /= BH1750_CONV_FACTOR;

// Print converted value if debug enabled
#ifdef BH1750_DEBUG
    Serial.print(F("[BH1750] Converted float value: "));
    Serial.println(level);
#endif
  }

  return level;
}

/**
 * Power down BH1750 sensor
 * @param none
 * @return 1 if mode successful set,
 * 				 0 if mode not changed or parameter out of range
 */
uint8_t power_down(void){
	uint8_t ack = 4;
	uint8_t 	DataTransmitToBH1750[1];
	DataTransmitToBH1750[0] = BH1750_POWER_DOWN;
	uint16_t Size = sizeof(DataTransmitToBH1750);
	uint32_t Timeout = 1000;
	
	ack = HAL_I2C_Master_Transmit(&hi2c1, BH1750_I2CADDR, DataTransmitToBH1750, Size, Timeout);

  // Wait a few moments
  _delay_ms(15);

  // Check result code
  switch (ack) {
		case 0:
			//size = sprintf((char *)Data, "BH1750: Status I2C - HAL_OK\n\r");	
			//HAL_UART_Transmit(&huart3, Data, size, 1000);
			return 1;
		case 1: // HAL_ERROR
			size = sprintf((char *)Data, "BH1750:power_down error HAL_ERROR\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 2: // HAL_BUSY
			size = sprintf((char *)Data, "BH1750: error HAL_BUSY\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 3: // HAL_TIMEOUT
			size = sprintf((char *)Data, "BH1750: error HAL_TIMEOUT\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		default:
			size = sprintf((char *)Data, "BH1750: undefined error\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
	}

  return 0;
}

/**
 * Power on BH1750 sensor
 * @param none
 * @return 1 if mode successful set,
 * 				 0 if mode not changed or parameter out of range
 */
uint8_t power_on(void){
	uint8_t ack = 4;
	uint8_t 	DataTransmitToBH1750[1];
	DataTransmitToBH1750[0] = BH1750_POWER_ON;
	uint16_t Size = sizeof(DataTransmitToBH1750);
	uint32_t Timeout = 1000;
	
	ack = HAL_I2C_Master_Transmit(&hi2c1, BH1750_I2CADDR, DataTransmitToBH1750, Size, Timeout);

  // Wait a few moments
  _delay_ms(15);

  // Check result code
  switch (ack) {
		case 0:
			//size = sprintf((char *)Data, "BH1750: Status I2C - HAL_OK\n\r");	
			//HAL_UART_Transmit(&huart3, Data, size, 1000);
			return 1;
		case 1: // HAL_ERROR
			size = sprintf((char *)Data, "BH1750:power_on error HAL_ERROR\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 2: // HAL_BUSY
			size = sprintf((char *)Data, "BH1750: error HAL_BUSY\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		case 3: // HAL_TIMEOUT
			size = sprintf((char *)Data, "BH1750: error HAL_TIMEOUT\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
		default:
			size = sprintf((char *)Data, "BH1750: undefined error\n\r");	
			HAL_UART_Transmit(&huart3, Data, size, 1000);
			break;
	}

  return 0;
}



//-------------------------------- EOF -----------------------------------------
