
#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "MMC_SD.h"

class TFTSDTouch{
public:
	Adafruit_SPITFT tft;
	MMC_SD			mmc;
	uint32_t OpenReadFile(uint8_t Xpoz, uint16_t Ypoz, const char* BmpName);
	uint32_t CopyFile(const char* BmpName1, const char* BmpName2);
	uint32_t GetDirectoryBitmapFiles(const char* DirName, char* Files[]);
	uint32_t CheckBitmapFile(const char* BmpName, uint32_t* FileLen);
	uint8_t Buffercmp(uint8_t* pBuffer1, uint8_t* pBuffer2, uint16_t BufferLength);
	void Show_bmp(uint8_t Bmp_ScanDir, uint8_t Lcd_ScanDir);

	TFTSDTouch();// { tft = TFTSDTouch::Adafruit_SPITFT(); }
protected:
	char* pDirectoryFiles[MAX_BMP_FILES];
	uint8_t str[20];
	FRESULT f_res;
	unsigned char file_name[25][11];
	uint8_t aBuffer[1440];/* 480 * 3 = 1440 */
	FILINFO MyFileInfo;
	DIR MyDirectory;
	FIL MyFile;
	UINT BytesWritten;
	UINT BytesRead;
	uint16_t pic[76800];
};

extern uint8_t id;

