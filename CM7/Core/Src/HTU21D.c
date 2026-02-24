/*

  This is a library for the HTU21D Digital Humidity and Temperature Sensor.

  The HTU21D board uses I2C for communication. Two pins are required to
  interface to the device. Configuring the I2C bus is expected to be done
  in user code. The HTU21D library doesn't do this automatically.

  Written by Kulikov Vladimir, January, 2026.

*/

#include "HTU21D.h"
#include "stm32h7xx_hal.h"

extern I2C_HandleTypeDef hi2c1;

/*******************************************************************************
* Sends a 'reset' request to the HTU21D, followed by a 15ms delay.
*******************************************************************************/
void HTU21D_Soft_Reset(void){
	uint8_t DataTransmitToHTU21D[1];
	DataTransmitToHTU21D[0] = HTU21D_RESET_CMD;
	uint16_t Size = sizeof(DataTransmitToHTU21D);
	uint32_t Timeout = 1000;
	HAL_I2C_Master_Transmit(&hi2c1, HTU21D_IIC_ADDR, DataTransmitToHTU21D, Size, Timeout);
	HAL_Delay(15);
}


/*******************************************************************************
* Performs a single temperature conversion in degrees Celsius.
*
* @return a single-precision (32-bit) float value indicating the measured
*         temperature in degrees Celsius or NAN on failure.
*******************************************************************************/
float HTU21D_Temperature_Measurement(void){
	uint8_t DataTransmitToHTU21D[1];
	uint8_t	DataReceiveFromHTU21D[3];
	DataTransmitToHTU21D[0] = HTU21D_TRIG_T_HOLD_CMD;
	uint16_t Size = sizeof(DataTransmitToHTU21D);
	uint32_t Timeout = 1000;
	HAL_I2C_Master_Transmit(&hi2c1, HTU21D_IIC_ADDR, DataTransmitToHTU21D, Size, Timeout);
	
	// Read three bytes from the sensor, which are low and high parts of the sensor value and CRC
	HAL_I2C_Master_Receive(&hi2c1, HTU21D_IIC_ADDR, DataReceiveFromHTU21D, Size, Timeout);
	unsigned int tmp = 0;
	float temperature;
	tmp = DataReceiveFromHTU21D[0];
	tmp <<= 8;
	tmp |= DataReceiveFromHTU21D[1] & 0b11111100;	// Read 14 bit
	
	// 3rd byte is the CRC
	
	temperature = tmp;
  temperature *= 175.72f;
  temperature /= 65536.0f;
  temperature -= 46.85f;
	
	return temperature;
}


/*******************************************************************************
* Performs a single relative humidity conversion.
*
* @return A single-precision (32-bit) float value indicating the relative
*         humidity in percent (0..100.0%).
*******************************************************************************/
float HTU21D_Humidity_Measurement(void){
	uint8_t DataTransmitToHTU21D[1];
	uint8_t	DataReceiveFromHTU21D[3];
	DataTransmitToHTU21D[0] = HTU21D_TRIG_H_HOLD_CMD;
	uint16_t Size = sizeof(DataTransmitToHTU21D);
	uint32_t Timeout = 1000;
	HAL_I2C_Master_Transmit(&hi2c1, HTU21D_IIC_ADDR, DataTransmitToHTU21D, Size, Timeout);
	
	// Read three bytes from the sensor, which are low and high parts of the sensor value and CRC
	HAL_I2C_Master_Receive(&hi2c1, HTU21D_IIC_ADDR, DataReceiveFromHTU21D, Size, Timeout);
	unsigned int tmp = 0;
	float humidity;
	tmp = DataReceiveFromHTU21D[0];
	tmp <<= 8;
	tmp |= DataReceiveFromHTU21D[1] & 0b11111100;
	
	// 3rd byte is the CRC
	
	humidity = tmp;
  humidity *= 125.0f;
  humidity /= 65536.0f;
  humidity -= 6.0f;
	
	return humidity;
}


/*******************************************************************************
* Performs a single relative humidity compensation.
*
* @return A single-precision (32-bit) float value indicating the relative
*         compensated humidity in percent (0..100.0%).
*******************************************************************************/
float HTU21D_Humidity_CompensatedT(float Temp, float Humd){
	float f_Compensated_Humidity;
	if(Humd > 0 && Temp > 0 && Temp < 80){
		f_Compensated_Humidity = Humd + (25 - Temp) * TEMP_COEFFICIENT;
		return f_Compensated_Humidity;
	}else if(Humd <= 0){
		return 0.00;
	}else return Humd;
}


/*******************************************************************************
* Function Name  :	htu21_checkCRC8
* Description    : 	Calculates Data CRC8 for 16bit received Data
* Input          : 	16-bit data
* Output         : 	None
* Return         : 	8-bit data
* Attention		 : 	None
*******************************************************************************/
uint8_t HTU21D_checkCRC8(uint16_t data){
	
	uint8_t bit;

	for (bit = 0; bit < 16; bit++)
	{
		if (data & 0x8000)
		{
			data = (data << 1) ^ CRC8_POLYNOMINAL;
		}
		else
		{
			data <<= 1;
		}
	}
	data >>= 8;

	return data;
}



//-------------------------------- EOF -----------------------------------------
