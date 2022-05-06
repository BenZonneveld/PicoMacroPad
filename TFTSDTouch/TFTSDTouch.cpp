#include <ff.h>
//#include <diskio.h>
#include <string.h>

#include "GFX-lib/Adafruit_GFX.h"
#include "GFX-lib/Adafruit_SPITFT.h"
#include "mmc_sd/MMC_SD.h"
#include "TFTSDTouch.h"

#define RGB24TORGB16(R,G,B) ((R>>3)<<11)|((G>>2)<<5)|(B>>3)
#define PIXEL(__M)  ((((__M) + 31 ) >> 5) << 2)//

TFTSDTouch::TFTSDTouch()
{
	tft = Adafruit_SPITFT();
	mmc = MMC_SD();

}

/**
* @brief  Open a file and copy its content to a buffer
* @param  DirName: the Directory name to open
* @param  FileName: the file name to open
* @param  BufferAddress: A pointer to a buffer to copy the file to
* @param  FileLen: the File length
* @retval err: Error status (0=> success, 1=> fail)
*/

uint32_t TFTSDTouch::OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char* BmpName)
{
	uint16_t i, j, k, h;

	uint32_t index = 0, size = 0, width = 0, height = 0;
	uint32_t bmpaddress, bit_pixel = 0;
	FIL file1;

	mmc.f_open(&file1, BmpName, FA_READ);
	mmc.f_read(&file1, aBuffer, 30, &BytesRead);

	bmpaddress = (uint32_t)aBuffer;

	/* Read bitmap size */
	size = *(uint16_t*)(bmpaddress + 2);
	size |= (*(uint16_t*)(bmpaddress + 4)) << 16;
	//	printf("file size =  %d \r\n",size);
		/* Get bitmap data address offset */
	index = *(uint16_t*)(bmpaddress + 10);
	index |= (*(uint16_t*)(bmpaddress + 12)) << 16;
	// printf("file index =  %d \r\n",index);
	/* Read bitmap width */
	width = *(uint16_t*)(bmpaddress + 18);
	width |= (*(uint16_t*)(bmpaddress + 20)) << 16;
	// printf("file width =  %d \r\n",width);
	/* Read bitmap height */
	height = *(uint16_t*)(bmpaddress + 22);
	height |= (*(uint16_t*)(bmpaddress + 24)) << 16;
	// printf("file height =  %d \r\n",height);
	/* Read bit/pixel */
	bit_pixel = *(uint16_t*)(bmpaddress + 28);
	//	printf("bit_pixel = %d \r\n",bit_pixel);
	mmc.f_close(&file1);

	if (24 != bit_pixel) {
		return 0;
	}

	//	if (width != sLCD_DIS.LCD_Dis_Column || height != sLCD_DIS.LCD_Dis_Page) {
			// printf("width != sLCD_DIS.LCD_Dis_Column \r\n");
			// printf("file width =  %d \r\n",width);
			// printf("file height =  %d \r\n",height);
			// printf("sLCD_DIS.LCD_Dis_Column =  %d \r\n",sLCD_DIS.LCD_Dis_Column);
			// printf("sLCD_DIS.LCD_Dis_Page =  %d \r\n",sLCD_DIS.LCD_Dis_Page);
	return 1;
	//	}

		/* Synchronize f_read right in front of the image data */
	mmc.f_open(&file1, (TCHAR const*)BmpName, FA_READ);
	mmc.f_read(&file1, aBuffer, index, &BytesRead);

	for (i = 0; i < height; i++) {
		mmc.f_read(&file1, aBuffer, 360, (UINT*)&BytesRead);
		mmc.f_read(&file1, aBuffer + 360, 360, (UINT*)&BytesRead);
		for (j = 0; j < width; j++) {
			k = j * 3;
			pic[i * 240 + j] = (uint16_t)(((aBuffer[k + 2] >> 3) << 11) | ((aBuffer[k + 1] >> 2) << 5) | (aBuffer[k] >> 3));
		}
	}
	/* LCD_SetCursor if dont write here ,it will display innormal*/
//		LCD_SetCursor(0, 0);
	gpio_put(LCD_DC_PIN, 1);
	gpio_put(LCD_CS_PIN, 0);
	spi_set_baudrate(spi1, 30 * 1000 * 1000);
	for (index = 0; index < 76800; index++) {
		mmc.SPI4W_Write_Byte((pic[index] >> 8) & 0xFF);
		mmc.SPI4W_Write_Byte(pic[index] & 0xFF);
	}
	gpio_put(LCD_CS_PIN, 1);
	mmc.f_close(&file1);
	spi_set_baudrate(spi1, 3000 * 1000);
	sleep_ms(1500);
	return 1;
}


/**
* @brief  Copy file BmpName1 to BmpName2
* @param  BmpName1: the source file name
* @param  BmpName2: the destination file name
* @retval err: Error status (0=> success, 1=> fail)
*/
uint32_t TFTSDTouch::CopyFile(const char* BmpName1, const char* BmpName2)
{
	uint32_t index = 0;
	FIL file1, file2;

	/* Open an Existent BMP file system */
	mmc.f_open(&file1, BmpName1, FA_READ);
	/* Create a new BMP file system */
	mmc.f_open(&file2, BmpName2, FA_CREATE_ALWAYS | FA_WRITE);

	do
	{
		mmc.f_read(&file1, aBuffer, _MAX_SS, &BytesRead);
		mmc.f_write(&file2, aBuffer, _MAX_SS, &BytesWritten);
		index += _MAX_SS;

	} while (index < file1.fsize);

	mmc.f_close(&file1);
	mmc.f_close(&file2);

	return 1;
}

/**
* @brief  Opens a file and copies its content to a buffer.
* @param  DirName: the Directory name to open
* @param  FileName: the file name to open
* @param  BufferAddress: A pointer to a buffer to copy the file to
* @param  FileLen: File length
* @retval err: Error status (0=> success, 1=> fail)
*/
uint32_t TFTSDTouch::CheckBitmapFile(const char* BmpName, uint32_t* FileLen)
{
	uint32_t err = 0;
	if (mmc.f_open(&MyFile, BmpName, FA_READ) != FR_OK) {
		err = 2;
	}
	mmc.f_close(&MyFile);
	return err;
}

/**
* @brief  List up to 25 file on the root directory with extension .BMP
* @param  DirName: Directory name
* @param  Files: Buffer to contain read files
* @retval The number of the found files
*/
uint32_t TFTSDTouch::GetDirectoryBitmapFiles(const char* DirName, char* Files[])
{
	uint32_t i = 0, j = 0;
	FRESULT res;

	res = mmc.f_opendir(&MyDirectory, DirName);
	if (res == FR_OK) {
		i = strlen(DirName);
		for (;;) {
			res = mmc.f_readdir(&MyDirectory, &MyFileInfo);
			if (res != FR_OK || MyFileInfo.fname[0] == 0) break;
			if (MyFileInfo.fname[0] == '.') continue;
			if (!(MyFileInfo.fattrib & AM_DIR)) {
				do {
					i++;
				} while (MyFileInfo.fname[i] != 0x2E);
				if (j < MAX_BMP_FILES) {
					if ((MyFileInfo.fname[i + 1] == 'B') && (MyFileInfo.fname[i + 2] == 'M') && (MyFileInfo.fname[i + 3] == 'P')) {
						sprintf(Files[j], "%-11.11s", MyFileInfo.fname);
						j++;
					}
				}
				i = 0;
			}
		}
	}
	return j;
}

/**
  * @brief  Compares two buffers.
  * @param  pBuffer1, pBuffer2: buffers to be compared
  * @param  BufferLength: buffer's length
  * @retval  0: pBuffer1 identical to pBuffer2
  *          1: pBuffer1 differs from pBuffer2
  */
uint8_t TFTSDTouch::Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength)
{
	uint8_t ret = 1;
	while (BufferLength--)
	{
		if (*pBuffer1 != *pBuffer2)
		{
			ret = 0;
		}

		pBuffer1++;
		pBuffer2++;
	}

	return ret;
}

/********************************************************************************
function:	Display the BMP picture in the SD card
parameter:
		Bmp_ScanDir :   Displays the LCD scanning mode of the BMP picture
		Lcd_ScanDir :   LCD normal display scan
********************************************************************************/
void TFTSDTouch::Show_bmp(uint8_t Bmp_ScanDir, uint8_t Lcd_ScanDir) {
	uint32_t bmplen = 0x00;
	uint32_t checkstatus = 0x00;
	uint32_t filesnumbers = 0x00;
	uint32_t bmpcounter = 0x00;
	DIR directory;
	FRESULT res;

	/* Open directory */
	//LCD_Clear(LCD_BACKGROUND);
	res = mmc.f_opendir(&directory, "/");
	if ((res != FR_OK)) {
		if (res == FR_NO_FILESYSTEM) {
			/* Display message: SD card not FAT formated */
			tft.setCursor(0, 32);
			tft.println("SD_CARD_NOT_FORMATTED");
			//GUI_DisString_EN(0, 32, "SD_CARD_NOT_FORMATTED", &Font24, LCD_BACKGROUND, BLUE);
		}
		else {
			/* Display message: Fail to open directory */
			tft.setCursor(0, 48);
			tft.println("SD_CARD_OPEN_FAIL");
			//GUI_DisString_EN(0, 48, "SD_CARD_OPEN_FAIL", &Font24, LCD_BACKGROUND, BLUE);
		}
	}
	else {
		//printf("file open \r\n");
	}

	/* Get number of bitmap files */
	filesnumbers = GetDirectoryBitmapFiles("/", pDirectoryFiles);

	/* Set bitmap counter to display first image */
	bmpcounter = 1;

	int showtime;
	for (showtime = 0; showtime < filesnumbers; showtime++) {
		sprintf((char*)str, "%-11.11s", pDirectoryFiles[bmpcounter - 1]);

		checkstatus = CheckBitmapFile((const char*)str, &bmplen);

		if (checkstatus == 0) {
			//Display the scan of the picture
			//LCD_SetGramScanWay(Bmp_ScanDir);

			/* Open the image and display the picture */
			OpenReadFile(0, 0, (const char*)str);
		}
		else if (checkstatus == 1) {
			/* Display message: SD card does not exist */
			//Restore the default scan
//			LCD_SetGramScanWay(Lcd_ScanDir);
//			GUI_DisString_EN(0, 64, "SD_CARD_NOT_FOUND", &Font24, LCD_BACKGROUND, BLUE);
		}
		else {
			/* Display message: File not supported */
			//Restore the default scan
//			LCD_SetGramScanWay(Lcd_ScanDir);
//			GUI_DisString_EN(0, 80, "SD_CARD_FILE_NOT_SUPPORTED", &Font24, LCD_BACKGROUND, BLUE);
		}

		bmpcounter++;
		if (bmpcounter > filesnumbers) {
			bmpcounter = 1;
			break;
		}

	}
	//	LCD_Clear(LCD_BACKGROUND);
	//	//Restore the default scan
	//	LCD_SetGramScanWay(Lcd_ScanDir);
	gpio_put(SD_CS_PIN, 1);
}
