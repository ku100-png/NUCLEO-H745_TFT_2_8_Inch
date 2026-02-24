// author Kulikov V.G.

#include "iocontrol.h"
#include "jsmn.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>


extern Wifi_t	Wifi;
extern UART_HandleTypeDef huart3;
extern uint16_t size;
extern uint8_t Data[256];

// errors and hhtp_status
enum {
	httpOk = 200,
	emptyJson = 601,
	failedJsonRoot = 602,
	noType = 603,
	emptyBoard = 604,
	nothingToWrite = 606,
  nothingToRead = 607,
	invalidHeader = 701,
	intervalError = 702,
	invalidResponse = 703,
	invalidStatus = 704,
	connectionFailed = 801,
	invalidName = 1003
};

typedef struct
{
	uint8_t check[STR_LENGTH];
	uint8_t value[NUM_LENGTH];
	uint8_t date[NUM_LENGTH];
	uint8_t dateUnix[NUM_LENGTH];
	uint8_t message[STR_LENGTH];
	uint8_t requestTime[NUM_LENGTH];
}ReadJsonStruct;

ReadJsonStruct ReadStruct;

typedef struct
{
	uint8_t value[NUM_LENGTH];
	uint8_t check[STR_LENGTH];
	uint8_t message[STR_LENGTH];
	uint8_t requestTime[NUM_LENGTH];
}SendJsonStruct;

SendJsonStruct SendStruct;

int resultCode;
jsmn_parser p;
jsmntok_t tokens[MAXNUMBER_OF_TOKENS];

char get[] = "GET ";
char server_IP[] = "188.124.36.57";
char server_name[] = "https://iocontrol.ru";
//char panel_name[] = "first_panel/";
//char variable_name[] = "Illumination_Lux/";
char key[] = "?key=xsjhwikerw4buzpxv8ll";
char header[] = " HTTP/1.1\r\nHost: www.iocontrol.ru\r\nUser-Agent: arduino-ethernet\r\nConnection: close";
char headerEnd[] = "\r\n\r\n";


//#########################################################################################################
// Чтение величины int_16 с панели на сайте https://iocontrol.ru
bool ReadInt_from_iocontrol(const char* Panel_name, const char* Var_name)
{
	uint16_t status;
	char http_req[150];
	uint8_t BuffString[150];
	
	status = Wifi_TcpIp_StartTcpConnection(0, server_IP, HTTP_PORT, 10);
	if(!status){
		size = sprintf((char *)Data, "ReadInt_from_iocontrol: Wifi_TcpIp_StartTcpConnection ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(http_req,0,150);
	
	strcat(http_req, get);
	strcat(http_req, server_name);
	strcat(http_req, "/api");
	strcat(http_req, "/readData/");
	strcat(http_req, Panel_name);
	strcat(http_req, "/");
	strcat(http_req, Var_name);
	strcat(http_req, key);
	strcat(http_req, header);
	strcat(http_req, headerEnd);
	
	uint8_t len = strlen(http_req);
	
	status = Wifi_TcpIp_SendDataUdp(0, len, (uint8_t*)http_req);
	if(!status){
		size = sprintf((char *)Data, "ReadInt_from_iocontrol: Wifi_TcpIp_SendDataUdp ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	DataRecieved(Wifi.RxBuffer);
	
	status = findStringUntil(BuffString, Wifi.RxBuffer, '+', '\r', 256);
	status = httpStatus(BuffString);
	
	if(status != httpOk){
		size = sprintf((char *)Data, "ReadInt_from_iocontrol: error http status %d\n\r", status);	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(BuffString,0,150);
	status = findStringUntil(BuffString, Wifi.RxBuffer, '{', '}', 256);
	
	uint16_t len1=sizeof(BuffString);
	 
	jsmn_init(&p);
	resultCode = jsmn_parse(&p, (const char*)BuffString, len1, tokens, sizeof(tokens) / sizeof(tokens[0]));
	 
	if (resultCode > 0)
	{
		char keyString[MAX_TOKEN_LENGTH];
		char Prev_keyString[MAX_TOKEN_LENGTH];
		 
		for (int i = 1; i <= resultCode - 1; i++) // resultCode == 0 => whole json string
		{
			jsmntok_t key = tokens[i];
			uint16_t length = key.end - key.start;
	 
			if (length < MAX_TOKEN_LENGTH)
			{
				memcpy(keyString, &BuffString[key.start], length);
				keyString[length] = '\0';
				 
				if (strcmp(Prev_keyString, "check") == 0)
					strcpy((char*)ReadStruct.check, keyString);
				else if (strcmp(Prev_keyString, "value") == 0)
					strcpy((char*)ReadStruct.value, keyString);
				else if (strcmp(Prev_keyString, "date") == 0)
					strcpy((char*)ReadStruct.date, keyString);
				else if (strcmp(Prev_keyString, "dateUnix") == 0)
					strcpy((char*)ReadStruct.dateUnix, keyString);
				else if (strcmp(Prev_keyString, "message") == 0)
					strcpy((char*)ReadStruct.message, keyString);
				else if (strcmp(Prev_keyString, "requestTime") == 0)
					strcpy((char*)ReadStruct.requestTime, keyString);
				 
				strcpy(Prev_keyString, keyString);
			}
		}
	}
			
	if(strcmp((const char*)ReadStruct.check, "true") != 0){
		size = sprintf((char *)Data,"ReadInt_from_iocontrol: error check status %s\n\r", ReadStruct.check);
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}

	return true;
}

//#########################################################################################################
// Чтение величины float с панели на сайте https://iocontrol.ru
bool ReadFloat_from_iocontrol(const char* Panel_name, const char* Var_name)
{
	uint16_t status;
	char http_req[150];
	uint8_t BuffString[150];
	
	status = Wifi_TcpIp_StartTcpConnection(0, server_IP, HTTP_PORT, 10);
	if(!status){
		size = sprintf((char *)Data, "ReadFloat_from_iocontrol: Wifi_TcpIp_StartTcpConnection ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(http_req,0,150);
	
	strcat(http_req, get);
	strcat(http_req, server_name);
	strcat(http_req, "/api");
	strcat(http_req, "/readData/");
	strcat(http_req, Panel_name);
	strcat(http_req, "/");
	strcat(http_req, Var_name);
	strcat(http_req, key);
	strcat(http_req, header);
	strcat(http_req, headerEnd);
	
	uint8_t len = strlen(http_req);
	
	status = Wifi_TcpIp_SendDataUdp(0, len, (uint8_t*)http_req);
	if(!status){
		size = sprintf((char *)Data, "ReadFloat_from_iocontrol: Wifi_TcpIp_SendDataUdp ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	DataRecieved(Wifi.RxBuffer);
	
	status = findStringUntil(BuffString, Wifi.RxBuffer, '+', '\r', 256);
	status = httpStatus(BuffString);
	
	if(status != httpOk){
		size = sprintf((char *)Data, "ReadFloat_from_iocontrol: error http status %d\n\r", status);	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(BuffString,0,150);
	status = findStringUntil(BuffString, Wifi.RxBuffer, '{', '}', 256);
	
	uint16_t len1=sizeof(BuffString);
	 
	jsmn_init(&p);
	resultCode = jsmn_parse(&p, (const char*)BuffString, len1, tokens, sizeof(tokens) / sizeof(tokens[0]));
	 
	if (resultCode > 0)
	{
		char keyString[MAX_TOKEN_LENGTH];
		char Prev_keyString[MAX_TOKEN_LENGTH];
		 
		for (int i = 1; i <= resultCode - 1; i++) // resultCode == 0 => whole json string
		{
			jsmntok_t key = tokens[i];
			uint16_t length = key.end - key.start;
	 
			if (length < MAX_TOKEN_LENGTH)
			{
				memcpy(keyString, &BuffString[key.start], length);
				keyString[length] = '\0';
				 
				if (strcmp(Prev_keyString, "check") == 0)
					strcpy((char*)ReadStruct.check, keyString);
				else if (strcmp(Prev_keyString, "value") == 0)
					strcpy((char*)ReadStruct.value, keyString);
				else if (strcmp(Prev_keyString, "date") == 0)
					strcpy((char*)ReadStruct.date, keyString);
				else if (strcmp(Prev_keyString, "dateUnix") == 0)
					strcpy((char*)ReadStruct.dateUnix, keyString);
				else if (strcmp(Prev_keyString, "message") == 0)
					strcpy((char*)ReadStruct.message, keyString);
				else if (strcmp(Prev_keyString, "requestTime") == 0)
					strcpy((char*)ReadStruct.requestTime, keyString);
				 
				strcpy(Prev_keyString, keyString);
			}
		}
	}
			
	if(strcmp((const char*)ReadStruct.check, "true") != 0){
		size = sprintf((char *)Data,"ReadFloat_from_iocontrol: error check status %s\n\r", ReadStruct.check);
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	size = sprintf((char *)Data,"ReadFloat_from_iocontrol: %s\n\r", ReadStruct.value);
	HAL_UART_Transmit(&huart3, Data, size, 1000);

	return true;
}

//#########################################################################################################
// Отправка величины float на панель сайта https://iocontrol.ru
bool SendInt_to_iocontrol(const char* Panel_name, const char* Var_name, uint16_t var_d){
	
	uint16_t status;
	char http_req[150];
	char str_var_d[10];
	uint8_t BuffString[150];
	
	sprintf(str_var_d, "%d", var_d);
	
	status = Wifi_TcpIp_StartTcpConnection(0, server_IP, HTTP_PORT, 10);
	if(!status){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: Wifi_TcpIp_StartTcpConnection ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(http_req,0,150);
	
	strcat(http_req, get);
	strcat(http_req, server_name);
	strcat(http_req, "/api");
	strcat(http_req, "/sendData/");
	strcat(http_req, Panel_name);
	strcat(http_req, "/");
	strcat(http_req, Var_name);
	strcat(http_req, "/");
	strcat(http_req, str_var_d);
	strcat(http_req, key);
	strcat(http_req, header);
	strcat(http_req, headerEnd);
	
	uint8_t len = strlen(http_req);
	status = Wifi_TcpIp_SendDataUdp(0, len, (uint8_t*)http_req);
	if(!status){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: Wifi_TcpIp_SendDataUdp ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	DataRecieved(Wifi.RxBuffer);
	
	status = findStringUntil(BuffString, Wifi.RxBuffer, '+', '\r', 256);
	status = httpStatus(BuffString);
	
	if(status != httpOk){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: error http status %d\n\r", status);	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(BuffString,0,150);
	status = findStringUntil(BuffString, Wifi.RxBuffer, '{', '}', 256);
	
	uint16_t len1=sizeof(BuffString);
	 
	jsmn_init(&p);
	resultCode = jsmn_parse(&p, (const char*)BuffString, len1, tokens, sizeof(tokens) / sizeof(tokens[0]));
	 
	if (resultCode > 0)
	{
		char keyString[MAX_TOKEN_LENGTH];
		char Prev_keyString[MAX_TOKEN_LENGTH];
		 
		for (int i = 1; i <= resultCode - 1; i++) // resultCode == 0 => whole json string
		{
			jsmntok_t key = tokens[i];
			uint16_t length = key.end - key.start;
	 
			if (length < MAX_TOKEN_LENGTH)
			{
				memcpy(keyString, &BuffString[key.start], length);
				keyString[length] = '\0';
					
				if (strcmp(Prev_keyString, "value") == 0)
					strcpy((char*)SendStruct.value, keyString);
				else if (strcmp(Prev_keyString, "check") == 0)
					strcpy((char*)SendStruct.check, keyString);
				else if (strcmp(Prev_keyString, "message") == 0)
					strcpy((char*)SendStruct.message, keyString);
				else if (strcmp(Prev_keyString, "requestTime") == 0)
					strcpy((char*)SendStruct.requestTime, keyString);

				strcpy(Prev_keyString, keyString);
			}
		}
	}
			
	if(strcmp((const char*)SendStruct.check, "true") != 0){
		size = sprintf((char *)Data,"SendFloat_to_iocontrol: error check status %s\n\r", SendStruct.check);
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}

	return true;
}

//#########################################################################################################
// Отправка величины float на панель сайта https://iocontrol.ru
bool SendFloat_to_iocontrol(const char* Panel_name, const char* Var_name, float var_f){
	
	uint16_t status;
	char http_req[150];
	char str_var_f[10];
	uint8_t BuffString[150];
	
	sprintf(str_var_f, "%.2f", (float)var_f);
	
	status = Wifi_TcpIp_StartTcpConnection(0, server_IP, HTTP_PORT, 10);
	if(!status){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: Wifi_TcpIp_StartTcpConnection ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(http_req,0,150);
	
	strcat(http_req, get);
	strcat(http_req, server_name);
	strcat(http_req, "/api");
	strcat(http_req, "/sendData/");
	strcat(http_req, Panel_name);
	strcat(http_req, "/");
	strcat(http_req, Var_name);
	strcat(http_req, "/");
	strcat(http_req, str_var_f);
	strcat(http_req, key);
	strcat(http_req, header);
	strcat(http_req, headerEnd);
	
	uint8_t len = strlen(http_req);
	status = Wifi_TcpIp_SendDataUdp(0, len, (uint8_t*)http_req);
	if(!status){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: Wifi_TcpIp_SendDataUdp ERROR\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	DataRecieved(Wifi.RxBuffer);
	
	status = findStringUntil(BuffString, Wifi.RxBuffer, '+', '\r', 256);
	status = httpStatus(BuffString);
	
	if(status != httpOk){
		size = sprintf((char *)Data, "SendFloat_to_iocontrol: error http status %d\n\r", status);	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}
	
	memset(BuffString,0,150);
	status = findStringUntil(BuffString, Wifi.RxBuffer, '{', '}', 256);
	
	uint16_t len1=sizeof(BuffString);
	 
	jsmn_init(&p);
	resultCode = jsmn_parse(&p, (const char*)BuffString, len1, tokens, sizeof(tokens) / sizeof(tokens[0]));
	 
	if (resultCode > 0)
	{
		char keyString[MAX_TOKEN_LENGTH];
		char Prev_keyString[MAX_TOKEN_LENGTH];
		 
		for (int i = 1; i <= resultCode - 1; i++) // resultCode == 0 => whole json string
		{
			jsmntok_t key = tokens[i];
			uint16_t length = key.end - key.start;
	 
			if (length < MAX_TOKEN_LENGTH)
			{
				memcpy(keyString, &BuffString[key.start], length);
				keyString[length] = '\0';
					
				if (strcmp(Prev_keyString, "value") == 0)
					strcpy((char*)SendStruct.value, keyString);
				else if (strcmp(Prev_keyString, "check") == 0)
					strcpy((char*)SendStruct.check, keyString);
				else if (strcmp(Prev_keyString, "message") == 0)
					strcpy((char*)SendStruct.message, keyString);
				else if (strcmp(Prev_keyString, "requestTime") == 0)
					strcpy((char*)SendStruct.requestTime, keyString);

				strcpy(Prev_keyString, keyString);
			}
		}
	}
			
	if(strcmp((const char*)SendStruct.check, "true") != 0){
		size = sprintf((char *)Data,"SendFloat_to_iocontrol: error check status %s\n\r", SendStruct.check);
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		return false;
	}

	return true;
}

//#########################################################################################################
// Проверка статуса подключения. Подключено - 3, отключено - 4.
bool IsDisconnected(void){
	uint8_t status;
	uint8_t Timeout = 0;
	
	do{
		Timeout++;
		Wifi_TcpIp_GetConnectionStatus();
		status = Wifi.TcpIpConnections[0].status;
		if(Timeout > 100){
			Wifi_TcpIp_Close(0);
			break;
		}
		HAL_Delay(1);
	}while(status == 3);

	size = sprintf((char *)Data,"Timeout: %d\r\n", Timeout);
	HAL_UART_Transmit(&huart3, Data, size, 1000);
	
	return true;
}

//#########################################################################################################
// Копирует в buffEx символы из buffIn, пока не встретит символ symbol
// Возвращает номер символа symbol
uint16_t readStringUntil(uint8_t* buffEx, uint8_t* buffIn, uint8_t symbol, uint16_t maxLength){
	uint16_t n = 0;
	
	uint16_t len_buffIn = strlen((const char*)buffIn);
	if(len_buffIn > 0) maxLength = len_buffIn;
	
	while(n < maxLength){
		if(buffIn[n] == symbol) break;
		buffEx[n] = buffIn[n];
		n++;
	}
	buffEx[n+1] = '\r';
	buffEx[n+1] = '\n';
	
	return n;
}
//#########################################################################################################
// Ищет в buffIn symbol и возвращает его номер
uint16_t findString(uint8_t* buffIn, uint8_t symbol, uint16_t maxLength){
	uint16_t n = 0;
	
	uint16_t len_buffIn = strlen((const char*)buffIn);
	if(len_buffIn > 0) maxLength = len_buffIn;
	
	while(n < maxLength){
		if(buffIn[n] == symbol){
			//return n;
			break;
		}
		n++;
	}

	return n;
}
//#########################################################################################################
// Ищет в buffIn symbol с номера num и возвращает его номер
uint16_t findStringFrom(uint8_t* buffIn, uint8_t symbol, uint16_t num, uint16_t maxLength){
	uint16_t n = num;
	
	uint16_t len_buffIn = strlen((const char*)buffIn);
	if(len_buffIn > 0) maxLength = len_buffIn;
	
	while(n < maxLength){
		if(buffIn[n] == symbol){
			//return n;
			break;
		}
		n++;
	}

	return n;
}
//#########################################################################################################
// Копирует в buffEx строку с символа symbol1 по symbol2 из buffIn
uint16_t findStringUntil(uint8_t* buffEx, uint8_t* buffIn, uint8_t symbol1, uint8_t symbol2, uint16_t maxLength){
	uint16_t n1 = 0;
	uint16_t n2 = 0;
	uint16_t n3 = 0;
	
	n1 = findString(buffIn, symbol1, maxLength);
	n2 = findStringFrom(buffIn, symbol2, n1, maxLength);
	n3 = n2 - (n1-1);
	
	uint16_t i, j;
	j = 0;
	for(i = n1; i <= n2; i++)
  {
		buffEx[j] = buffIn[i];
		j++;
  }
	
//	buffEx[j+1] = '\r';
//	buffEx[j+2] = '\n';
	
	return n3;
}
//#########################################################################################################
// Возвращает статус http запроса
uint16_t httpStatus(uint8_t* buffIn){
	uint16_t n1 = 0;
	uint16_t n2 = strlen((const char*)buffIn);
	uint16_t n3 = 0;
	uint8_t http_stat[5];
	
	n1 = findString(buffIn, '/', n2);
	
	uint16_t i, j;
	j = 0;
	
	for(i = n1+4; i < n1+8; i++)
  {
		http_stat[j] = buffIn[i];
		j++;
	}
	
	n3 = atoi((const char*)http_stat);
	
	return n3;
}
//#########################################################################################################
// Возвращает True, если в буфер приема больше не поступают данные
bool DataRecieved(uint8_t* buffIn){
	uint16_t DataLen= 0;
	uint16_t Rx_len;
	bool fl_status = true;
	uint16_t timeout;
	
	Rx_len = strlen((const char*)buffIn);
	if(!Rx_len){
		HAL_Delay(1);
		Rx_len = strlen((const char*)buffIn);
	}
	
	while(Rx_len != DataLen){
		
		DataLen = Rx_len;
		HAL_Delay(30);
		Rx_len = strlen((const char*)buffIn);
		timeout++;
		if(timeout > 100){
			fl_status = false;
			break;
		}
	}
	return fl_status;
}









//-------------------------------- EOF -----------------------------------------


