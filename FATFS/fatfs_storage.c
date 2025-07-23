/**
  ******************************************************************************
  * @file    fatfs_storage.c
  * @author  Ku100
  * @version V1.0.0
  * @date    22-July-2025
  * @brief   This file includes the Storage (FatsFs) driver 
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT 2014 STMicroelectronics</center></h2>
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include <string.h>

#include "LCD.h"
#include <stdio.h>
#include "fatfs_storage.h"
#include "ff.h"			/* Declarations of FatFs API */

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
#define BITMAP_BUFFER_SIZE 1*512 /* You can adapt this size depending on the amount of RAM available */
#define BITMAP_HEADER_SIZE sizeof(BmpHeader) /* Bitmap specificity */
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
uint8_t aBuffer[BITMAP_HEADER_SIZE + BITMAP_BUFFER_SIZE];
FILINFO MyFileInfo;
DIR MyDirectory;
FIL MyFile;
UINT BytesWritten, BytesRead;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/

/**
* @brief  Open a file and copy its content to a buffer
* @param  DirName: the Directory name to open
* @param  FileName: the file name to open
* @param  BufferAddress: A pointer to a buffer to copy the file to
* @param  FileLen: the File length
* @retval err: Error status (0=> success, 1=> fail)
*/
#define PIXEL(__M)  ((((__M) + 31 ) >> 5) << 2)
uint32_t Storage_OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char* BmpName)
{
    uint16_t i, j, k;
    
    uint32_t index = 0, size = 0, width = 0, height = 0;
    uint32_t bmpaddress, bit_pixel = 0;
    FIL file1; 
    uint16_t color = 0;

    f_open(&file1, BmpName, FA_READ);
    f_read(&file1, aBuffer, 30, &BytesRead);

    bmpaddress = (uint32_t)aBuffer;

    /* Read bitmap size */
    size = *(uint16_t *) (bmpaddress + 2);
    size |= (*(uint16_t *) (bmpaddress + 4)) << 16;

    /* Get bitmap data address offset */
    index = *(uint16_t *) (bmpaddress + 10);
    index |= (*(uint16_t *) (bmpaddress + 12)) << 16;

    /* Read bitmap width */
    width = *(uint16_t *) (bmpaddress + 18);
    width |= (*(uint16_t *) (bmpaddress + 20)) << 16;

    /* Read bitmap height */
    height = *(uint16_t *) (bmpaddress + 22);
    height |= (*(uint16_t *) (bmpaddress + 24)) << 16;

    /* Read bit/pixel */
    bit_pixel = *(uint16_t *) (bmpaddress + 28);  
    f_close (&file1);

    if (24 != bit_pixel) {
        return 0;
    }
    
    /* Synchronize f_read right in front of the image data */
    f_open (&file1, (TCHAR const*)BmpName, FA_READ);  
    f_read(&file1, aBuffer, index, &BytesRead);
    
    for (i = 0; i < 320; i ++) {
			
        f_read(&file1, aBuffer, PIXEL(240 * bit_pixel) >> 1, (UINT *)&BytesRead);
        f_read(&file1, aBuffer + (PIXEL(240 * bit_pixel) >> 1), PIXEL(240 * bit_pixel) >> 1, (UINT *)&BytesRead);
        lcd_set_cursor(Xpoz + 0, Ypoz + i);
        for (j = 0; j < 240; j ++) {
            k = j * 3; 
            color = (uint16_t)(((aBuffer[k + 2] >> 3) << 11 ) | ((aBuffer[k + 1] >> 2) << 5) | (aBuffer[k] >> 3));
            lcd_write_word(color);
        }
    } 
    f_close(&file1);
    return 1;
}


/**
  * @brief  Copy file BmpName1 to BmpName2
  * @param  BmpName1: the source file name
  * @param  BmpName2: the destination file name
  * @retval err: Error status (0=> success, 1=> fail)
  */
uint32_t Storage_CopyFile(const char* BmpName1, const char* BmpName2)
{
  FRESULT fr = FR_OK;
  FIL file1, file2;

  /* Open an Existent BMP file system */
  fr = f_open(&file1, BmpName1, FA_READ);
  if (fr)
    return (uint32_t)fr;
  /* Create a new BMP file system */
  fr = f_open(&file2, BmpName2, FA_CREATE_ALWAYS | FA_WRITE);
  if (fr)
     return (uint32_t)fr;
  for(;;)
  {
    fr = f_read(&file1, aBuffer, _MAX_SS, &BytesRead);
    if (fr || BytesRead == 0)
      break; /* error or eof */
    fr = f_write(&file2, aBuffer, _MAX_SS, &BytesWritten);
    if (fr || BytesWritten < _MAX_SS)
      break; /* error or buffer full */
  }

  f_close(&file1);
  f_close(&file2);

  return (uint32_t)fr;
}

/**
  * @brief  Opens a file and copies its content to a buffer.
  * @param  DirName: the Directory name to open
  * @param  FileName: the file name to open
  * @param  BufferAddress: A pointer to a buffer to copy the file to
  * @param  FileLen: File length
  * @retval err: Error status (0=> success, 1=> fail)
  */
uint32_t Storage_CheckBitmapFile(const char* BmpName, uint32_t *FileLen)
{
  uint32_t err = 0;

  if(f_open(&MyFile, BmpName, FA_READ) != FR_OK)
  {
    err = 1;
  }
  else
  {
    if(f_close(&MyFile) != FR_OK)
    {
      err = 1;
    }
  }

  return err;
}

/**
  * @brief  List up to 25 file on the root directory with extension .BMP
  * @param  DirName: Directory name
  * @param  Files: Buffer to contain read files
  * @retval The number of the found files
  */
uint32_t Storage_GetDirectoryBitmapFiles(const char* DirName, char* Files[])
{
  FRESULT res;
  uint32_t index = 0;

  /* Start to search for wave files */
  res = f_findfirst(&MyDirectory, &MyFileInfo, DirName, "*.bmp");

  /* Repeat while an item is found */
  while (MyFileInfo.fname[0])
  {
    if(res == FR_OK)
    {
      if(index < MAX_BMP_FILES)
      {
				sprintf (Files[index++], "%s", MyFileInfo.fname);
      }
      /* Search for next item */
      res = f_findnext(&MyDirectory, &MyFileInfo);
    }
    else
    {
      index = 0;
      break;
    }
  }

  f_closedir(&MyDirectory);

  return index;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared
  * @param  BufferLength: buffer's length
  * @retval  0: pBuffer1 identical to pBuffer2
  *          1: pBuffer1 differs from pBuffer2
  */
uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
  uint8_t ret = 1;

  while (BufferLength--)
  {
    if(*pBuffer1 != *pBuffer2)
    {
      ret = 0;
    }

    pBuffer1++;
    pBuffer2++;
  }

  return ret;
}



/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
