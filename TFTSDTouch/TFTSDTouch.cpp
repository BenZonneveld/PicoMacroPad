#include <ff.h>
#include <string.h>

#include "GFX-lib/Adafruit_GFX.h"
#include "GFX-lib/Adafruit_SPITFT.h"
#include "mmc_sd/MMC_SD.h"
//#include "ImageReader/ImageReader.h"
#include "Touch/Touch.h"
#include "TFTSDTouch.h"

#define RGB24TORGB16(R,G,B) ((R>>3)<<11)|((G>>2)<<5)|(B>>3)
#define PIXEL(__M)  ((((__M) + 31 ) >> 5) << 2)//

TFTSDTouch::TFTSDTouch()
{
	tft = Adafruit_SPITFT();
	mmc = MMC_SD();
	TP = cTouch();
//	mmc.Init();
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
//	reader.drawBMP(BmpName, tft, Xpoz, Ypoz);
	sleep_ms(1500);
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
	filesnumbers = GetDirectoryBitmapFiles("/", mmc.pDirectoryFiles);

	/* Set bitmap counter to display first image */
	bmpcounter = 1;

	tft.print(filesnumbers);
	tft.println(" files found.");

	int showtime;
	for (showtime = 0; showtime < filesnumbers; showtime++) {
		sprintf((char*)str, "%-11.11s", mmc.pDirectoryFiles[bmpcounter - 1]);

		checkstatus = CheckBitmapFile((const char*)str, &bmplen);

		if (checkstatus == 0) {
			//Display the scan of the picture
			//LCD_SetGramScanWay(Bmp_ScanDir);

			/* Open the image and display the picture */
			OpenReadFile(0, 0, (const char*)str);
			tft.println((const char*)str);
		}
		else if (checkstatus == 1) {
			/* Display message: SD card does not exist */
			//Restore the default scan
//			LCD_SetGramScanWay(Lcd_ScanDir); 
//			GUI_DisString_EN(0, 64, "SD_CARD_NOT_FOUND", &Font24, LCD_BACKGROUND, BLUE);
			tft.setCursor(0, 64);
			tft.print("SD_CARD_NOT_FOUND");
		}
		else {
			/* Display message: File not supported */
			//Restore the default scan
//			LCD_SetGramScanWay(Lcd_ScanDir);
//			GUI_DisString_EN(0, 80, "SD_CARD_FILE_NOT_SUPPORTED", &Font24, LCD_BACKGROUND, BLUE);
			tft.setCursor(0, 80);
			tft.print("SD_CARD_FILE_NOT_SUPPORTED");
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

