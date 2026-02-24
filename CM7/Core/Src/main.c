/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "string.h"
#include <stdio.h>
#include "fatfs.h"
#include "fatfs_sd.h"
#include "fatfs_storage.h"
#include "BH1750.h"
#include "HTU21D.h"
#include "BMP280.h"
#include "ESP8266.h"
#include "iocontrol.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#ifndef HSEM_ID_0
#define HSEM_ID_0 (0U) /* HW semaphore 0*/
#endif


// ============================================================================
#define    DWT_CYCCNT    *(volatile uint32_t *)0xE0001004
#define    DWT_CONTROL   *(volatile uint32_t *)0xE0001000
#define    SCB_DEMCR     *(volatile uint32_t *)0xE000EDFC


static void 
DWT_Init()
{
  SCB_DEMCR  |= 0x01000000;
  DWT_CYCCNT  = 0;
  DWT_CONTROL|= 1; // enable the counter
}

#define ITM_Port8(n)    (*((volatile unsigned char *)(0xE0000000+4*n)))
#define ITM_Port16(n)   (*((volatile unsigned short*)(0xE0000000+4*n)))
#define ITM_Port32(n)   (*((volatile unsigned long *)(0xE0000000+4*n)))

#define DEMCR           (*((volatile unsigned long *)(0xE000EDFC)))
#define TRCENA          0x01000000

struct __FILE { int handle; };
FILE __stdout;
FILE __stdin;

int 
fputc(int ch, FILE *f) 
{
 if (DEMCR & TRCENA) 
{
   while (ITM_Port32(0) == 0);
   ITM_Port8(0) = ch;
 }
 return(ch);
}


/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
#if defined ( __ICCARM__ ) /*!< IAR Compiler */

#pragma location=0x30040000
ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
#pragma location=0x30040060
ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
#pragma location=0x300400c0
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffers */

#elif defined ( __CC_ARM )  /* MDK ARM Compiler */

__attribute__((at(0x30040000))) ETH_DMADescTypeDef  DMARxDscrTab[ETH_RX_DESC_CNT]; /* Ethernet Rx DMA Descriptors */
__attribute__((at(0x30040060))) ETH_DMADescTypeDef  DMATxDscrTab[ETH_TX_DESC_CNT]; /* Ethernet Tx DMA Descriptors */
__attribute__((at(0x300400c0))) uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE]; /* Ethernet Receive Buffer */

#elif defined ( __GNUC__ ) /* GNU Compiler */

ETH_DMADescTypeDef DMARxDscrTab[ETH_RX_DESC_CNT] __attribute__((section(".RxDecripSection"))); /* Ethernet Rx DMA Descriptors */
ETH_DMADescTypeDef DMATxDscrTab[ETH_TX_DESC_CNT] __attribute__((section(".TxDecripSection")));   /* Ethernet Tx DMA Descriptors */
uint8_t Rx_Buff[ETH_RX_DESC_CNT][ETH_MAX_PACKET_SIZE] __attribute__((section(".RxArraySection"))); /* Ethernet Receive Buffers */

#endif

ETH_TxPacketConfig TxConfig;

ETH_HandleTypeDef heth;

SPI_HandleTypeDef hspi1;

I2C_HandleTypeDef hi2c1;

UART_HandleTypeDef huart3;
UART_HandleTypeDef huart6;

PCD_HandleTypeDef hpcd_USB_OTG_FS;

ADC_HandleTypeDef hadc1;

/* USER CODE BEGIN PV */
char* pDirectoryFiles[MAX_BMP_FILES];
uint8_t str[20];
FRESULT fres;
FIL fil;
uint8_t fl_file_opened;
FATFS SD_FatFs;  /* File system object for SD card logical drive */
char SD_Path[4]; /* SD card logical drive path */

uint8_t DemoIndex = 0;

BMP280_HandleTypedef bmp280;
float pressure, temperature, humidity, pressure_mm;
uint16_t size;
uint8_t Data[512];
uint8_t BuffString[256];

extern Wifi_t	Wifi;


BSP_DemoTypedef  BSP_examples[]=
{
  {Demo_DrawGraphic, 	"DrawGraphic", 	0},
  {Demo_ShowImages, 	"ShowImages", 	0},
  {Demo_Touchscreen, 	"Touchscreen", 	0},
	{Demo_SDwriteFile, 	"SDwriteFile", 	0},
	{Demo_ClimatParam, 	"ClimatParam",	0},
};
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_ETH_Init(void);
static void MX_SPI1_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_USART6_UART_Init(void);
static void MX_USB_OTG_FS_PCD_Init(void);
static void MX_ADC1_Init(void);
/* USER CODE BEGIN PFP */
static void display_images(void);
static void SDCard_Config(void);
//static void SDCard_Test(void);
uint16_t map(uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max);

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	
  /* USER CODE END 1 */
	/* USER CODE BEGIN Boot_Mode_Sequence_0 */
  int32_t timeout;
	/* USER CODE END Boot_Mode_Sequence_0 */

	/* USER CODE BEGIN Boot_Mode_Sequence_1 */
  /* Wait until CPU2 boots and enters in stop mode or timeout*/
  timeout = 0xFFFF;
  while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) != RESET) && (timeout-- > 0));
  if ( timeout < 0 )
  {
  Error_Handler();
  }
	/* USER CODE END Boot_Mode_Sequence_1 */
  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();
	/* USER CODE BEGIN Boot_Mode_Sequence_2 */
	/* When system initialization is finished, Cortex-M7 will release Cortex-M4 by means of
	HSEM notification */
	/*HW semaphore Clock enable*/
	__HAL_RCC_HSEM_CLK_ENABLE();
	/*Take HSEM */
	HAL_HSEM_FastTake(HSEM_ID_0);
	/*Release HSEM in order to notify the CPU2(CM4)*/
	HAL_HSEM_Release(HSEM_ID_0,0);
	/* wait until CPU2 wakes up from stop mode */
	timeout = 0xFFFF;
	while((__HAL_RCC_GET_FLAG(RCC_FLAG_D2CKRDY) == RESET) && (timeout-- > 0));
	if ( timeout < 0 )
	{
	Error_Handler();
	}
	/* USER CODE END Boot_Mode_Sequence_2 */

  /* USER CODE BEGIN SysInit */
	DWT_Init();
	
	printf("***** Start debug ***\r\n");
	printf("Init complete\r\n");
  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_ETH_Init();
  MX_SPI1_Init();
	MX_I2C1_Init();
  MX_USART3_UART_Init();
	MX_USART6_UART_Init();
  MX_USB_OTG_FS_PCD_Init();
	MX_ADC1_Init();
	MX_FATFS_Init();
	DWT_Delay_Init();
  
	/* Initialize User_Button on STM32H7xx-Nucleo ------------------*/
  BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);
	
	/* USER CODE BEGIN 2 */
	HAL_Delay(1000);
	driver_init();	// LCD Init
	
	/* Configure SD card */
	SDCard_Config();
	
	HAL_GPIO_WritePin(LED2_GPIO, LED2_PIN, GPIO_PIN_SET);
	HAL_GPIO_WritePin(LED3_GPIO, LED3_PIN, GPIO_PIN_SET);
	
	lcd_display_string(50, 16, (const uint8_t *)"STM32H745ZI-Q", 16, RED);
	lcd_display_string(20, 48, (const uint8_t *)"Drivers examples:", 16, RED);
	
	for(uint8_t i=0; i<5; i++)
  {
		lcd_display_string(50, 64+(16*i), (const uint8_t *)BSP_examples[DemoIndex].DemoName, 16, RED);
		DemoIndex++;
  }
	lcd_display_string(20, 160, (const uint8_t *)"Press User Button to start", 16, RED);
	
	/* 0. Wait for User button to be pressed -------------------------------------*/
  while(CheckForUserInput() == 0)
  {}
	DemoIndex = 4;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */
		BSP_examples[DemoIndex].DemoFunc();
		HAL_Delay(10);
		HAL_GPIO_TogglePin(LED1_GPIO, LED1_PIN);
		HAL_GPIO_TogglePin(LED3_GPIO, LED3_PIN);		
    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
	RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Supply configuration update enable
  */
  HAL_PWREx_ConfigSupply(PWR_DIRECT_SMPS_SUPPLY);
  /** Configure the main internal regulator output voltage
  */
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}
  /** Macro to configure the PLL clock source
  */
  __HAL_RCC_PLL_PLLSOURCE_CONFIG(RCC_PLLSOURCE_HSE);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_BYPASS;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 1;
  RCC_OscInitStruct.PLL.PLLN = 100;
  RCC_OscInitStruct.PLL.PLLP = 2;
  RCC_OscInitStruct.PLL.PLLQ = 4;
  RCC_OscInitStruct.PLL.PLLR = 2;
  RCC_OscInitStruct.PLL.PLLRGE = RCC_PLL1VCIRANGE_3;
  RCC_OscInitStruct.PLL.PLLVCOSEL = RCC_PLL1VCOWIDE;
  RCC_OscInitStruct.PLL.PLLFRACN = 0;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                              |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV2;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV2;
  RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief ETH Initialization Function
  * @param None
  * @retval None
  */
static void MX_ETH_Init(void)
{

  /* USER CODE BEGIN ETH_Init 0 */

  /* USER CODE END ETH_Init 0 */

   static uint8_t MACAddr[6];

  /* USER CODE BEGIN ETH_Init 1 */

  /* USER CODE END ETH_Init 1 */
  heth.Instance = ETH;
  MACAddr[0] = 0x00;
  MACAddr[1] = 0x80;
  MACAddr[2] = 0xE1;
  MACAddr[3] = 0x00;
  MACAddr[4] = 0x00;
  MACAddr[5] = 0x00;
  heth.Init.MACAddr = &MACAddr[0];
  heth.Init.MediaInterface = HAL_ETH_RMII_MODE;
  heth.Init.TxDesc = DMATxDscrTab;
  heth.Init.RxDesc = DMARxDscrTab;
  heth.Init.RxBuffLen = 1524;

  /* USER CODE BEGIN MACADDRESS */

  /* USER CODE END MACADDRESS */

  if (HAL_ETH_Init(&heth) != HAL_OK)
  {
    Error_Handler();
  }

  memset(&TxConfig, 0 , sizeof(ETH_TxPacketConfig));
  TxConfig.Attributes = ETH_TX_PACKETS_FEATURES_CSUM | ETH_TX_PACKETS_FEATURES_CRCPAD;
  TxConfig.ChecksumCtrl = ETH_CHECKSUM_IPHDR_PAYLOAD_INSERT_PHDR_CALC;
  TxConfig.CRCPadCtrl = ETH_CRC_PAD_INSERT;
  /* USER CODE BEGIN ETH_Init 2 */

  /* USER CODE END ETH_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 0x0;
  hspi1.Init.NSSPMode = SPI_NSS_PULSE_ENABLE;
  hspi1.Init.NSSPolarity = SPI_NSS_POLARITY_LOW;
  hspi1.Init.FifoThreshold = SPI_FIFO_THRESHOLD_01DATA;
  hspi1.Init.TxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.RxCRCInitializationPattern = SPI_CRC_INITIALIZATION_ALL_ZERO_PATTERN;
  hspi1.Init.MasterSSIdleness = SPI_MASTER_SS_IDLENESS_00CYCLE;
  hspi1.Init.MasterInterDataIdleness = SPI_MASTER_INTERDATA_IDLENESS_00CYCLE;
  hspi1.Init.MasterReceiverAutoSusp = SPI_MASTER_RX_AUTOSUSP_DISABLE;
  hspi1.Init.MasterKeepIOState = SPI_MASTER_KEEP_IO_STATE_DISABLE;
  hspi1.Init.IOSwap = SPI_IO_SWAP_DISABLE;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.Timing = 0x10C0ECFF;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Analogue filter
  */
  if (HAL_I2CEx_ConfigAnalogFilter(&hi2c1, I2C_ANALOGFILTER_ENABLE) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Digital filter
  */
  if (HAL_I2CEx_ConfigDigitalFilter(&hi2c1, 0) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 115200;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  huart3.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart3.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart3.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart3, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart3, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.Init.ClockPrescaler = UART_PRESCALER_DIV1;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetTxFifoThreshold(&huart6, UART_TXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_SetRxFifoThreshold(&huart6, UART_RXFIFO_THRESHOLD_1_8) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_UARTEx_DisableFifoMode(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */
	__HAL_UART_ENABLE_IT(&huart6, UART_IT_RXNE);
	//__HAL_UART_ENABLE_IT(&huart6, UART_IT_IDLE);
  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief USB_OTG_FS Initialization Function
  * @param None
  * @retval None
  */
static void MX_USB_OTG_FS_PCD_Init(void)
{

  /* USER CODE BEGIN USB_OTG_FS_Init 0 */

  /* USER CODE END USB_OTG_FS_Init 0 */

  /* USER CODE BEGIN USB_OTG_FS_Init 1 */

  /* USER CODE END USB_OTG_FS_Init 1 */
  hpcd_USB_OTG_FS.Instance = USB_OTG_FS;
  hpcd_USB_OTG_FS.Init.dev_endpoints = 9;
  hpcd_USB_OTG_FS.Init.speed = PCD_SPEED_FULL;
  hpcd_USB_OTG_FS.Init.dma_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.phy_itface = PCD_PHY_EMBEDDED;
  hpcd_USB_OTG_FS.Init.Sof_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.low_power_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.lpm_enable = DISABLE;
  hpcd_USB_OTG_FS.Init.battery_charging_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.vbus_sensing_enable = ENABLE;
  hpcd_USB_OTG_FS.Init.use_dedicated_ep1 = DISABLE;
  if (HAL_PCD_Init(&hpcd_USB_OTG_FS) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USB_OTG_FS_Init 2 */

  /* USER CODE END USB_OTG_FS_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{

	GPIO_InitTypeDef GPIO_InitStruct = {0};
	
  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOG_CLK_ENABLE();
	__HAL_RCC_GPIOE_CLK_ENABLE();
	
	
	/* LCD GPIO Configuration
	PD14     ------> LCD_CS
	PG12		 ------> LCD_DC
	PD15     ------> LCD_BL
	*/
	GPIO_InitStruct.Pin = LCD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(LCD_CS_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_DC_PIN;
	HAL_GPIO_Init(LCD_DC_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LCD_BKL_PIN;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LCD_BKL_GPIO, &GPIO_InitStruct);

	/* SD GPIO Configuration
	PE11      ------> SD_CS
	*/
	GPIO_InitStruct.Pin = SD_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(SD_CS_GPIO_PORT, &GPIO_InitStruct);

	/* Touch GPIO Configuration
	PE14      ------> XPT2046_CS
	*/
	GPIO_InitStruct.Pin = XPT2046_CS_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_PULLUP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
	HAL_GPIO_Init(XPT2046_CS_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = XPT2046_IRQ_PIN;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(XPT2046_IRQ_GPIO, &GPIO_InitStruct);

	/* LED GPIO Configuration
	PB0      ------> LED1_Green
	PE1      ------> LED2_Yellow
	PB14		 ------> LED3_Red
	*/
	GPIO_InitStruct.Pin = LED1_PIN|LED3_PIN;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(LED1_GPIO, &GPIO_InitStruct);
	
	GPIO_InitStruct.Pin = LED2_PIN;
	HAL_GPIO_Init(LED2_GPIO, &GPIO_InitStruct);
	
	/* Capasitor sensor GPIO Configuration
	PC10      ------> GND
	PC12      ------> VCC
	*/
	GPIO_InitStruct.Pin = GPIO_PIN_10|GPIO_PIN_12;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_10, GPIO_PIN_RESET);	// PC10 -> Low
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_RESET);	// PC12 -> Low
}

/**
  * @brief ADC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_ADC1_Init(void)
{

  /* USER CODE BEGIN ADC1_Init 0 */

  /* USER CODE END ADC1_Init 0 */

  ADC_MultiModeTypeDef multimode = {0};
  ADC_ChannelConfTypeDef sConfig = {0};

  /* USER CODE BEGIN ADC1_Init 1 */

  /* USER CODE END ADC1_Init 1 */

  /** Common config
  */
  hadc1.Instance = ADC1;
  hadc1.Init.ClockPrescaler = ADC_CLOCK_ASYNC_DIV1;
  hadc1.Init.Resolution = ADC_RESOLUTION_16B;
  hadc1.Init.ScanConvMode = ADC_SCAN_DISABLE;
  hadc1.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
  hadc1.Init.LowPowerAutoWait = DISABLE;
  hadc1.Init.ContinuousConvMode = DISABLE;
  hadc1.Init.NbrOfConversion = 1;
  hadc1.Init.DiscontinuousConvMode = DISABLE;
  hadc1.Init.ExternalTrigConv = ADC_SOFTWARE_START;
  hadc1.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
  hadc1.Init.ConversionDataManagement = ADC_CONVERSIONDATA_DR;
  hadc1.Init.Overrun = ADC_OVR_DATA_PRESERVED;
  hadc1.Init.LeftBitShift = ADC_LEFTBITSHIFT_NONE;
  hadc1.Init.OversamplingMode = DISABLE;
  if (HAL_ADC_Init(&hadc1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure the ADC multi-mode
  */
  multimode.Mode = ADC_MODE_INDEPENDENT;
  if (HAL_ADCEx_MultiModeConfigChannel(&hadc1, &multimode) != HAL_OK)
  {
    Error_Handler();
  }

  /** Configure Regular Channel
  */
  sConfig.Channel = ADC_CHANNEL_18;
  sConfig.Rank = ADC_REGULAR_RANK_1;
  sConfig.SamplingTime = ADC_SAMPLETIME_32CYCLES_5;
  sConfig.SingleDiff = ADC_SINGLE_ENDED;
  sConfig.OffsetNumber = ADC_OFFSET_NONE;
  sConfig.Offset = 0;
  sConfig.OffsetSignedSaturation = DISABLE;
  if (HAL_ADC_ConfigChannel(&hadc1, &sConfig) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN ADC1_Init 2 */

  /* USER CODE END ADC1_Init 2 */

}

/* USER CODE BEGIN 4 */
/**
  * @brief  Output the image on LCD from SD.
  * @param  None
  * @retval None
  */
static void display_images(void)
{
	  uint32_t bmplen = 0x00;
    uint32_t checkstatus = 0x00;
    uint32_t filesnumbers = 0x00;
    uint32_t bmpcounter = 0x00;
    DIR directory;
    FRESULT res;
	/* Open directory */
    res= f_opendir(&directory, "/");
    if((res != FR_OK))
    {
      if(res == FR_NO_FILESYSTEM)
      {
        /* Display message: SD card not FAT formated */
        lcd_display_string(0, 32, (const uint8_t *)"SD_CARD_NOT_FORMATTED", 16, RED);  
          
      }
      else
      {
        /* Display message: Fail to open directory */
         lcd_display_string(0, 48, (const uint8_t *)"SD_CARD_OPEN_FAIL", 16, RED);           
      }
    }
    
    /* Get number of bitmap files */
    filesnumbers = Storage_GetDirectoryBitmapFiles ("/", pDirectoryFiles);    
		printf("filesnumbers %d\r\n",filesnumbers);
    /* Set bitmap counter to display first image */
    bmpcounter = 1; 
    
    while (1)
    {     
			printf("bmpcounter %d\r\n",bmpcounter);	
			
			sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter -1]);
			printf("str %s\r\n",str);
        
        checkstatus = Storage_CheckBitmapFile((const char*)str, &bmplen);
        
        if(checkstatus == 0)
        {
          /* Format the string */
          Storage_OpenReadFile(0, 0, (const char*)str); 
        }
        else if (checkstatus == 1)
        {
          /* Display message: SD card does not exist */
           lcd_display_string(0, 64, (const uint8_t *)"SD_CARD_NOT_FOUND", 16, RED);  
        }
        else
        {
          /* Display message: File not supported */
            lcd_display_string(0, 80, (const uint8_t *)"SD_CARD_FILE_NOT_SUPPORTED", 16, RED); 
        }
        
        bmpcounter ++;
        if(bmpcounter > filesnumbers)
        {
          bmpcounter = 1;
        }
				
				HAL_Delay(5000);
				
				if(DemoIndex>1){
					break;
				}
    }
}


/**
  * @brief  Configuration the SDcard.
  * @param  None
  * @retval None
  */
static void SDCard_Config(void)
{
  uint32_t counter = 0;
	
	/* Check the mounted device */
	if(f_mount(&SD_FatFs, (TCHAR const*)"/", 0) != FR_OK)
	{
		lcd_display_string(0, 16, (const uint8_t *)"FATFS_NOT_MOUNTED", 16, RED);
	}
	else
	{
		/* Initialize the Directory Files pointers (heap) */
		for (counter = 0; counter < MAX_BMP_FILES; counter++)
		{
			pDirectoryFiles[counter] = malloc(11);
		}
	}
}


/**
  * @brief  Test to write file on SDcard.
  * @param  None
  * @retval None
  */
static void SDCard_Test(void)
{
		fres = f_open(&fil, "log.txt", FA_OPEN_APPEND | FA_WRITE | FA_READ);
		if (fres == FR_OK) {
			printf("File log.txt opened.\n");
			lcd_display_string(0, 16, (const uint8_t *)"File log.txt opened", 16, BLACK);
		} else if (fres != FR_OK) {
			printf("File was not opened!\n");
			lcd_display_string(0, 16, (const uint8_t *)"FILE_NOT_OPENED", 16, RED);
		}
		
		for (uint8_t i = 0; i < 10; i++) {
			f_puts("This text is written in the file.\n", &fil);
		}
		
		if (fres == FR_OK) {
			lcd_display_string(0, 32, (const uint8_t *)"Text written in the file", 16, BLACK);
		}
		
		/* Close file */
		fres = f_close(&fil);
		if (fres == FR_OK) {
			printf("The file is closed.\n");
			lcd_display_string(0, 48, (const uint8_t *)"File log.txt closed", 16, BLACK);
		} else if (fres != FR_OK) {
			printf("The file was not closed.\n");
			lcd_display_string(0, 48, (const uint8_t *)"FILE_NOT_CLOSED", 16, RED);
		}
		
		lcd_display_string(0, 300, (const uint8_t *)"Press User Button to continue", 16, BLACK);
		
		while(1){
			if(CheckForUserInput()){
				break;
			}
		}
}


/**
  * @brief  Demo Draw Graphic.
  * @param  None
  * @retval None
  */
void Demo_DrawGraphic(void)
{
	lcd_clear_screen(BLACK);
	
	lcd_display_string(60, 120, (const uint8_t *)"Hello, world !", FONT_1608, WHITE);
  lcd_display_string(30, 152, (const uint8_t *)"2.8' TFT Touch Shield", FONT_1608, WHITE);
	
	lcd_draw_circle(50,50,30,BLUE);
	lcd_fill_rect(160,200,60,100,GREEN);
	lcd_draw_rect(80,90,100,100,YELLOW);
	lcd_draw_h_line(18,180,200,RED);
	lcd_draw_v_line(30,120,100,GRAY);
	
	lcd_display_string(0, 300, (const uint8_t *)"Press User Button to continue", 16, WHITE);
	
	while(1){
		if(CheckForUserInput()){
			break;
		}
	}
}

/**
  * @brief  Demo Show Images.
  * @param  None
  * @retval None
  */
void Demo_ShowImages(void)
{
	lcd_clear_screen(WHITE);
	display_images();
}

/**
  * @brief  Demo Touchscreen.
  * @param  None
  * @retval None
  */
void Demo_Touchscreen(void)
{
	lcd_clear_screen(WHITE);
	
	/* Slow SPI speed  */
	SET_SPI_BAUDRATEPRESCALER_128;
	/*-----------------*/
	
	tp_init();
	tp_adjust();
  tp_dialog();
	
	while(1){
		tp_draw_board();
		if(CheckForUserInput()){
			/* Fast SPI speed  */
			SET_SPI_BAUDRATEPRESCALER_32;
			/*-----------------*/
			
			break;
		}
	}
}

/**
  * @brief  Demo microSD write File.
  * @param  None
  * @retval None
  */
void Demo_SDwriteFile(void)
{
	lcd_clear_screen(WHITE);
	SDCard_Test();
}


/**
  * @brief  Demo Show climatic parameters.
  * @param  None
  * @retval None
  */
void Demo_ClimatParam(void)
{
	static uint8_t x_coordinat = 0;
	int16_t y_coordinat_l;
	static int16_t y_coordinat_l_old = 290;
	int16_t y_coordinat_t;
	static int16_t y_coordinat_t_old = 290;
	int16_t y_coordinat_h;
	static int16_t y_coordinat_h_old = 290;
	int16_t y_coordinat_c;
	static int16_t y_coordinat_c_old = 290;
	int16_t adc_value;
	int16_t adc_value_maped;
	uint8_t status;
	
	lcd_clear_screen(WHITE);
	lcd_display_string(30, 0, (const uint8_t *)"Climatic parameters:", 16, BLACK);
	lcd_display_string(0, 16, (const uint8_t *)"Cap soil sensor,%:", 16, BLACK);
	lcd_display_string(0, 32, (const uint8_t *)"Illumination, Lux:", 16, BLACK);
	lcd_display_string(0, 48, (const uint8_t *)"Temperature, C:", 16, BLACK);
	lcd_display_string(0, 64, (const uint8_t *)"Humidity, %:", 16, BLACK);
	lcd_display_string(145, 64, (const uint8_t *)"P, mm:", 16, BLACK);
	lcd_display_string(0, 300, (const uint8_t *)"Press User Button to continue", 16, BLACK);
	
	lcd_draw_h_line(0, 90, 240, GRAY);
	lcd_draw_h_line(0, 130, 240, GRAY);
	lcd_draw_h_line(0, 170, 240, GRAY);
	lcd_draw_h_line(0, 210, 240, GRAY);
	lcd_draw_h_line(0, 250, 240, GRAY);
	lcd_draw_h_line(0, 290, 240, BLACK);
	
	lcd_draw_v_line(40, 90, 202, GRAY);
	lcd_draw_v_line(80, 90, 202, GRAY);
	lcd_draw_v_line(120, 90, 202, GRAY);
	lcd_draw_v_line(160, 90, 202, GRAY);
	lcd_draw_v_line(200, 90, 202, GRAY);
	lcd_draw_v_line(0, 90, 200, BLACK);
	
	begin(BH1750_CONTIN_HR_MODE, BH1750_I2CADDR, &hi2c1);
	
	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_12, GPIO_PIN_SET);	// PC12 -> High
	
	//  Init BMP280
	bmp280_init_default_params(&bmp280.params);
	bmp280_init_i2c(&bmp280, BMP280_I2C_ADDRESS_0, &hi2c1);
	if(bmp280_init(&bmp280, &bmp280.params)){
		// BMP280 correct init
		size = sprintf((char *)Data, "BMP280: correct init\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
	}
	
	// Wi-fi init
	if(Wifi_Init()==false)
	{
		size = sprintf((char *)Data, "ESP8266: incorrect init\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
	}else{
		size = sprintf((char *)Data, "ESP8266: correct init\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
	}
	
	//WifiMode_t m = WifiMode_Station;
	Wifi_SetMode(WifiMode_Station);
	
	status = Wifi_Station_ConnectToAp("Umbrella_Corporation", "iD7qm3#1", NULL);
	
	if(!status){
		size = sprintf((char *)Data, "ESP8266: Not connected to Wi-fi\n\r");	
		HAL_UART_Transmit(&huart3, Data, size, 1000);
	}
	
	Wifi_GetMyIp();
	Wifi_TcpIp_SetMultiConnection(true);

	while(1){
		if(CheckForUserInput()){
			x_coordinat = 0;
			y_coordinat_l_old = 290;
			y_coordinat_t_old = 290;
			y_coordinat_h_old = 290;
			y_coordinat_c_old = 290;
			break;
		}
		
		float Lux = readLightLevel();
		float Temperature = HTU21D_Temperature_Measurement();
		float Humidity = HTU21D_Humidity_Measurement();
		
		bmp280_read_float(&bmp280, &temperature, &pressure, &humidity);
		pressure_mm = pressure*10/1333;
		//size = sprintf((char *)Data,"Pressure: %.2f Pa, Temperature: %.2f C, Pressure in mm: %.2f mm\n\r", pressure, temperature, pressure_mm);
		//HAL_UART_Transmit(&huart3, Data, size, 1000);
		
		HAL_ADC_Start(&hadc1);
		HAL_ADC_PollForConversion(&hadc1, 100);
		adc_value = HAL_ADC_GetValue(&hadc1);
		adc_value_maped = map(adc_value, 0, 65535, 100, 0);
		adc_value_maped = map(adc_value_maped, 32, 85, 0, 100);
		
		char Lux_value[10];
		sprintf(Lux_value, "%.1f", (float)Lux);
		lcd_fill_rect(150, 32, 70, 16, WHITE);
		lcd_display_string(150, 32, (const uint8_t *)Lux_value, 16, RED);
		
		char Temp_value[10];
		sprintf(Temp_value, "%.1f", (float)Temperature);
		lcd_fill_rect(150, 48, 70, 16, WHITE);
		lcd_display_string(150, 48, (const uint8_t *)Temp_value, 16, BLUE);
		
		char Humid_value[10];
		sprintf(Humid_value, "%.1f", (float)Humidity);
		lcd_fill_rect(100, 64, 40, 16, WHITE);
		lcd_display_string(100, 64, (const uint8_t *)Humid_value, 16, MAGENTA);
		
		char ADC_value[10];
		sprintf(ADC_value, "%d", (uint16_t)adc_value_maped);
		lcd_fill_rect(150, 16, 70, 16, WHITE);
		lcd_display_string(150, 16, (const uint8_t *)ADC_value, 16, BROWN);
		
		char P_value[10];
		sprintf(P_value, "%.1f", (float)pressure_mm);
		lcd_fill_rect(195, 64, 40, 16, WHITE);
		lcd_display_string(195, 64, (const uint8_t *)P_value, 16, GRAY);
		
		y_coordinat_l = (int16_t)(-1*(Lux*4/10 - 290));
		y_coordinat_t = (int16_t)(-1*(Temperature*4 - 290));
		y_coordinat_h = (int16_t)(-1*(Humidity*4/2 - 290));
		y_coordinat_c = (int16_t)(-1*(adc_value_maped*4/2 - 290));
		
		if(x_coordinat >= 240) x_coordinat = 0;
		
		if(y_coordinat_l < 90) y_coordinat_l = 90;
		if(y_coordinat_l > 290) y_coordinat_l = 290;
		
		if(y_coordinat_t < 90) y_coordinat_t = 90;
		if(y_coordinat_t > 290) y_coordinat_t = 290;
		
		if(y_coordinat_h < 90) y_coordinat_h = 90;
		if(y_coordinat_h > 290) y_coordinat_h = 290;
		
		if(y_coordinat_c < 90) y_coordinat_c = 90;
		if(y_coordinat_c > 290) y_coordinat_c = 290;
		
		if(x_coordinat >= 30 && x_coordinat < 40){
			lcd_fill_rect(x_coordinat+1, 91, 39-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 39-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 39-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 39-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 39-x_coordinat, 39, WHITE);
			
			lcd_fill_rect(40+1, 91, x_coordinat-30, 39, WHITE);
			lcd_fill_rect(40+1, 131, x_coordinat-30, 39, WHITE);
			lcd_fill_rect(40+1, 171, x_coordinat-30, 39, WHITE);
			lcd_fill_rect(40+1, 211, x_coordinat-30, 39, WHITE);
			lcd_fill_rect(40+1, 251, x_coordinat-30, 39, WHITE);
		}else if(x_coordinat >= 70 && x_coordinat < 80){
			lcd_fill_rect(x_coordinat+1, 91, 79-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 79-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 79-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 79-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 79-x_coordinat, 39, WHITE);
			
			lcd_fill_rect(80+1, 91, x_coordinat-70, 39, WHITE);
			lcd_fill_rect(80+1, 131, x_coordinat-70, 39, WHITE);
			lcd_fill_rect(80+1, 171, x_coordinat-70, 39, WHITE);
			lcd_fill_rect(80+1, 211, x_coordinat-70, 39, WHITE);
			lcd_fill_rect(80+1, 251, x_coordinat-70, 39, WHITE);
		}else if(x_coordinat >= 110 && x_coordinat < 120){
			lcd_fill_rect(x_coordinat+1, 91, 119-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 119-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 119-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 119-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 119-x_coordinat, 39, WHITE);
			
			lcd_fill_rect(120+1, 91, x_coordinat-110, 39, WHITE);
			lcd_fill_rect(120+1, 131, x_coordinat-110, 39, WHITE);
			lcd_fill_rect(120+1, 171, x_coordinat-110, 39, WHITE);
			lcd_fill_rect(120+1, 211, x_coordinat-110, 39, WHITE);
			lcd_fill_rect(120+1, 251, x_coordinat-110, 39, WHITE);
		}else if(x_coordinat >= 150 && x_coordinat < 160){
			lcd_fill_rect(x_coordinat+1, 91, 159-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 159-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 159-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 159-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 159-x_coordinat, 39, WHITE);
			
			lcd_fill_rect(160+1, 91, x_coordinat-150, 39, WHITE);
			lcd_fill_rect(160+1, 131, x_coordinat-150, 39, WHITE);
			lcd_fill_rect(160+1, 171, x_coordinat-150, 39, WHITE);
			lcd_fill_rect(160+1, 211, x_coordinat-150, 39, WHITE);
			lcd_fill_rect(160+1, 251, x_coordinat-150, 39, WHITE);
		}else if(x_coordinat >= 190 && x_coordinat < 200){
			lcd_fill_rect(x_coordinat+1, 91, 199-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 199-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 199-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 199-x_coordinat, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 199-x_coordinat, 39, WHITE);
			
			lcd_fill_rect(200+1, 91, x_coordinat-190, 39, WHITE);
			lcd_fill_rect(200+1, 131, x_coordinat-190, 39, WHITE);
			lcd_fill_rect(200+1, 171, x_coordinat-190, 39, WHITE);
			lcd_fill_rect(200+1, 211, x_coordinat-190, 39, WHITE);
			lcd_fill_rect(200+1, 251, x_coordinat-190, 39, WHITE);
		}else{
			lcd_fill_rect(x_coordinat+1, 91, 10, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 131, 10, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 171, 10, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 211, 10, 39, WHITE);
			lcd_fill_rect(x_coordinat+1, 251, 10, 39, WHITE);
		}
		
		lcd_draw_h_line(0, 90, 240, GRAY);
		lcd_draw_h_line(0, 130, 240, GRAY);
		lcd_draw_h_line(0, 170, 240, GRAY);
		lcd_draw_h_line(0, 210, 240, GRAY);
		lcd_draw_h_line(0, 250, 240, GRAY);
		lcd_draw_h_line(0, 290, 240, BLACK);
		
		lcd_draw_v_line(40, 90, 202, GRAY);
		lcd_draw_v_line(80, 90, 202, GRAY);
		lcd_draw_v_line(120, 90, 202, GRAY);
		lcd_draw_v_line(160, 90, 202, GRAY);
		lcd_draw_v_line(200, 90, 202, GRAY);
		lcd_draw_v_line(0, 90, 200, BLACK);
		
		lcd_draw_line(x_coordinat, y_coordinat_l_old, x_coordinat+2, y_coordinat_l, RED);
		lcd_draw_line(x_coordinat, y_coordinat_t_old, x_coordinat+2, y_coordinat_t, BLUE);
		lcd_draw_line(x_coordinat, y_coordinat_h_old, x_coordinat+2, y_coordinat_h, MAGENTA);
		lcd_draw_line(x_coordinat, y_coordinat_c_old, x_coordinat+2, y_coordinat_c, BROWN);
		
		x_coordinat = x_coordinat + 2;
		
		y_coordinat_l_old = y_coordinat_l;
		y_coordinat_t_old = y_coordinat_t;
		y_coordinat_h_old = y_coordinat_h;
		y_coordinat_c_old = y_coordinat_c;
		
		// Обмен данными с https://iocontrol.ru
		//status = ReadInt_from_iocontrol("first_panel", "uint8_1");
		
		status = SendInt_to_iocontrol("Climatica_panel", "Soil_Humidity", adc_value_maped);
		status = SendFloat_to_iocontrol("Climatica_panel", "Air_Humidity", Humidity);
		status = SendFloat_to_iocontrol("Climatica_panel", "Air_Temperature", Temperature);
		status = SendFloat_to_iocontrol("Climatica_panel", "Illumination_Lux", Lux);
		status = SendFloat_to_iocontrol("Climatica_panel", "Pressure_mm", pressure_mm);


		HAL_Delay(2000);
	}
}


/**
  * @brief  Check for user input.
  * @param  None
  * @retval Input state (1 : active / 0 : Inactive)
  */
uint8_t CheckForUserInput(void)
{
  if(BSP_PB_GetState(BUTTON_USER) != RESET)
  {
    HAL_Delay(10);
    while (BSP_PB_GetState(BUTTON_USER) != RESET);
    return 1 ;
  }
  return 0;
}


/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}


/**
  * @brief  BSP Push Button callback
  * @param  Button Specifies the pin connected EXTI line
  * @retval None
  */
void BSP_PB_Callback(Button_TypeDef Button){
	DemoIndex++;
	if(DemoIndex > 4) DemoIndex = 0;
}

/**
  * @brief  Функция пропорцианального переноса значения из старого диапазона в новый
  * @param  текущее значение, границы старого и нового диапазонов
  * @retval Значение для переноса в границы нового диапазона
  */
uint16_t map(uint16_t value, uint16_t in_min, uint16_t in_max, uint16_t out_min, uint16_t out_max){
	return(uint16_t)(value - in_min)*(out_max - out_min)/(in_max - in_min) + out_min;
}


void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if(huart->Instance == USART6){
		// USART6 завершил прием данных
		Wifi_RxCallBack();
	}
}

void HAL_UART_ErrorCallBack(UART_HandleTypeDef *huart){
		if(huart->Instance == USART6){
		// USART6 завершил прием данных
		size = sprintf((char *)Data,"USART6 ErrorCallBack\n\r");
		HAL_UART_Transmit(&huart3, Data, size, 1000);
		
		HAL_UART_Receive_IT(&huart6, &Wifi.usartBuff, 1);
	}
}


#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */

