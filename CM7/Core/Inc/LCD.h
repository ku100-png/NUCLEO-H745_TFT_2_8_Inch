/**
  ******************************************************************************
  * @file    LCD.h 
  * @author  Ku100
  * @version 
  * @date    15-July-2025
  * @brief   This file provides configurations for work with LCD.
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
#ifndef _LCD_H_
#define _LCD_H_

/* Includes ------------------------------------------------------------------*/
#include <math.h>
#include <stdlib.h>
#include "stm32h7xx_hal.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/

#define LCD_WIDTH  240
#define LCD_HEIGHT 320

#define FONT_1206  12
#define FONT_1608  16

/** 
  * @brief  LCD ID  
  */  
#define  ST7789V				0x52
#define  HX8347D				0x47

/**
  * @}
  */

#define WHITE						0xFFFF
#define BLACK						0x0000	  
#define BLUE						0x001F  
#define BRED						0XF81F
#define GRED						0XFFE0
#define GBLUE						0X07FF
#define RED							0xF800
#define MAGENTA					0xF81F
#define GREEN						0x07E0
#define CYAN						0x7FFF
#define YELLOW					0xFFE0
#define BROWN						0XBC40 
#define BRRED						0XFC07 
#define GRAY						0X8430


#define MAX(a, b)  (((a) > (b)) ? (a) : (b))
#define MIN(a, b)  (((a) < (b)) ? (a) : (b))	


#define LCD_CMD    0
#define LCD_DATA   1


#define MAX_BMP_FILES  25 

/** 
  * @brief  ST7789H2 Registers  
  */
#define ST7789H2_LCD_ID             0x04
#define ST7789H2_SLEEP_IN           0x10
#define ST7789H2_SLEEP_OUT          0x11
#define ST7789H2_PARTIAL_DISPLAY    0x12
#define ST7789H2_DISPLAY_INVERSION  0x21
#define ST7789H2_DISPLAY_ON         0x29
#define ST7789H2_WRITE_RAM          0x2C
#define ST7789H2_READ_RAM           0x2E
#define ST7789H2_CASET              0x2A
#define ST7789H2_RASET              0x2B
#define ST7789H2_VSCRDEF            0x33 /* Vertical Scroll Definition */
#define ST7789H2_VSCSAD             0x37 /* Vertical Scroll Start Address of RAM */   
#define ST7789H2_TEARING_EFFECT     0x35
#define ST7789H2_NORMAL_DISPLAY     0x36
#define ST7789H2_IDLE_MODE_OFF      0x38
#define ST7789H2_IDLE_MODE_ON       0x39
#define ST7789H2_COLOR_MODE         0x3A
#define ST7789H2_WRCABCMB           0x5E /* Write Content Adaptive Brightness Control */
#define ST7789H2_RDCABCMB           0x5F /* Read Content Adaptive Brightness Control */
#define ST7789H2_PORCH_CTRL         0xB2
#define ST7789H2_GATE_CTRL          0xB7
#define ST7789H2_VCOM_SET           0xBB
#define ST7789H2_DISPLAY_OFF        0xBD
#define ST7789H2_LCM_CTRL           0xC0
#define ST7789H2_VDV_VRH_EN         0xC2
#define ST7789H2_VDV_SET            0xC4
#define ST7789H2_VCOMH_OFFSET_SET   0xC5
#define ST7789H2_FR_CTRL            0xC6
#define ST7789H2_POWER_CTRL         0xD0
#define ST7789H2_PV_GAMMA_CTRL      0xE0
#define ST7789H2_NV_GAMMA_CTRL      0xE1


/* Exported functions ------------------------------------------------------- */



void lcd_init(void);
uint8_t lcd_read_id(void);
void lcd_write_byte(uint8_t chByte, uint8_t chCmd);
void lcd_set_cursor( uint16_t hwXpos, uint16_t hwYpos);
void lcd_write_ram_prepare(void);
void lcd_write_ram( uint16_t hwData);
void lcd_set_window( uint16_t hwXpos, uint16_t hwYpos, uint16_t hwWidth, uint16_t hwHeight);
void lcd_draw_point( uint16_t hwXpos, uint16_t hwYpos, uint16_t hwColor);
void lcd_clear_screen( uint16_t hwColor);
void lcd_display_char(	uint16_t hwXpos, //specify x position.
													uint16_t hwYpos, //specify y position.
													uint8_t chChr,   //a char is display.
													uint8_t chSize,  //specify the size of the char
													uint16_t hwColor); //specify the color of the char
void lcd_display_num(		uint16_t hwXpos,  //specify x position.
													uint16_t hwYpos, //specify y position.
													uint32_t chNum,  //a number is display.
													uint8_t chLen,   //length ot the number
													uint8_t chSize,  //specify the size of the number
													uint16_t hwColor); //specify the color of the number
void lcd_display_string(uint16_t hwXpos, //specify x position.
													uint16_t hwYpos,   //specify y position.
													const uint8_t *pchString,  //a pointer to string
													uint8_t chSize,    // the size of the string 
													uint16_t hwColor);  // specify the color of the string 
void lcd_draw_line( 		uint16_t hwXpos0, //specify x0 position.
													uint16_t hwYpos0, //specify y0 position.
													uint16_t hwXpos1, //specify x1 position.
													uint16_t hwYpos1, //specify y1 position.
													uint16_t hwColor); //specify the color of the line
void lcd_draw_circle( 	uint16_t hwXpos,  //specify x position.
													uint16_t hwYpos,  //specify y position.
													uint16_t hwRadius, //specify the radius of the circle.
													uint16_t hwColor);  //specify the color of the circle.
void lcd_fill_rect( uint16_t hwXpos,  //specify x position.
													uint16_t hwYpos,  //specify y position.
													uint16_t hwWidth, //specify the width of the rectangle.
													uint16_t hwHeight, //specify the height of the rectangle.
													uint16_t hwColor);  //specify the color of rectangle.
void lcd_draw_v_line( 	uint16_t hwXpos, //specify x position.
													uint16_t hwYpos, //specify y position. 
													uint16_t hwHeight, //specify the height of the vertical line.
													uint16_t hwColor);  //specify the color of the vertical line.
void lcd_draw_h_line(	uint16_t hwXpos, //specify x position.
													uint16_t hwYpos,  //specify y position. 
													uint16_t hwWidth, //specify the width of the horizonal line.
													uint16_t hwColor); //specify the color of the horizonal line.
void lcd_draw_rect( 	uint16_t hwXpos,  //specify x position.
													uint16_t hwYpos,  //specify y position.
													uint16_t hwWidth, //specify the width of the rectangle.
													uint16_t hwHeight, //specify the height of the rectangle.
													uint16_t hwColor);  //specify the color of rectangle.

void setRotation( uint8_t rotation);
void lcd_write_word( uint16_t hwData);

#endif
/*-------------------------------END OF FILE-------------------------------*/

