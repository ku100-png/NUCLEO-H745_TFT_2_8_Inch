// author Kulikov V.G.

#ifndef __iocontrol_h__
#define __iocontrol_h__

#include "stm32h7xx_hal.h"
#include "ESP8266.h"

#define HTTP_PORT 80

#define MAXANSWERLENGTH 256
#define MAXNUMBER_OF_TOKENS 32
#define STR_LENGTH 19
#define NUM_LENGTH 19
#define MAX_TOKEN_LENGTH 12


bool ReadInt_from_iocontrol(const char* Panel_name, const char* Var_name);
bool ReadFloat_from_iocontrol(const char* Panel_name, const char* Var_name);
bool SendInt_to_iocontrol(const char* Panel_name, const char* Var_name, uint16_t var);
bool SendFloat_to_iocontrol(const char* Panel_name, const char* Var_name, float var);
bool IsDisconnected(void);


uint16_t readStringUntil(uint8_t* buffEx, uint8_t* buffIn, uint8_t sym, uint16_t maxLength);
uint16_t findString(uint8_t* buffIn, uint8_t sym, uint16_t maxLength);
uint16_t findStringFrom(uint8_t* buffIn, uint8_t sym, uint16_t num, uint16_t maxLength);
uint16_t findStringUntil(uint8_t* buffEx, uint8_t* buffIn, uint8_t sym1, uint8_t sym2, uint16_t maxLength);
uint16_t httpStatus(uint8_t* buffIn);
bool DataRecieved(uint8_t* buffIn);




#endif
//-------------------------------- EOF -----------------------------------------


