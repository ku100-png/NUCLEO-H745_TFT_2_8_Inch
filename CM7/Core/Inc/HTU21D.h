/*

  This is a library for the HTU21D Digital Humidity and Temperature Sensor.

  The HTU21D board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The HTU21D library doesn't do this automatically.

  Written by Kulikov Vladimir, January, 2026.

*/

#ifndef HTU21D_h
#define HTU21D_h

#include "stm32h7xx_hal.h"

// I2C Address HTU21D
#define HTU21D_IIC_ADDR 				0x80  // 0b  1000 0000 (учитывается и бит, отвечающий за R/W. Без него 7-битный адресс 0x40)

// Commands for HTU21D
#define HTU21D_RESET_CMD        0xFE
#define HTU21D_CFG_READ_CMD     0xE7
#define HTU21D_CFG_WRITE_CMD    0xE6
#define HTU21D_TRIG_T_CMD       0xF3
#define HTU21D_TRIG_H_CMD       0xF5
#define HTU21D_TRIG_T_HOLD_CMD  0xE3
#define HTU21D_TRIG_H_HOLD_CMD  0xE5

// Варианты разрешения датчика
#define HTU21D_RES_HIGH     0b00000000 // RH = 12Bit, T = 14Bit
#define HTU21D_RES_MEDIUM   0b10000001 // RH = 11Bit, T = 11Bit
#define HTU21D_RES_LOW      0b00000001 // RH = 8Bit, T = 12Bit

// Default values
#define TEMP_COEFFICIENT 							-0.15 	/* Temperature coefficient (from 0deg.C to 80deg.C) */
#define CRC8_POLYNOMINAL 							0x13100 /* CRC8 polynomial for 16bit CRC8 x^8 + x^5 + x^4 + 1 */

// Prototype functions
float HTU21D_Temperature_Measurement(void);
float HTU21D_Humidity_Measurement(void);
void HTU21D_Soft_Reset(void);
float HTU21D_Humidity_CompensatedT(float, float);
uint8_t HTU21D_checkCRC8(uint16_t data);


#endif
