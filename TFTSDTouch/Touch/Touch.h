/*****************************************************************************
* | File      	:	LCD_Touch.h
* | Author      :   Waveshare team
* | Function    :	LCD Touch Pad Driver and Draw
* | Info        :
*   Image scanning
*      Please use progressive scanning to generate images or fonts
*----------------
* |	This version:   V1.0
* | Date        :   2017-08-16
* | Info        :   Basic version
*
******************************************************************************/
#ifndef __LCD_TOUCH_H_
#define __LCD_TOUCH_H_

#include <math.h>
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/float.h"
#include "MMC_SD.h"
#include "Adafruit_SPITFT.h"

#define	COLOR				uint16_t		//The variable type of the color (unsigned short) 
#define	POINT				uint16_t		//The type of coordinate (unsigned short) 
#define	LENGTH				uint16_t		//The type of coordinate (unsigned short) 

#define TP_PRESS_DOWN           0x80
#define TP_PRESSED              0x40

typedef enum {
	DOT_PIXEL_1X1 = 1,		// dot pixel 1 x 1
	DOT_PIXEL_2X2, 		// dot pixel 2 X 2
	DOT_PIXEL_3X3,		// dot pixel 3 X 3
	DOT_PIXEL_4X4,		// dot pixel 4 X 4
	DOT_PIXEL_5X5, 		// dot pixel 5 X 5
	DOT_PIXEL_6X6, 		// dot pixel 6 X 6
	DOT_PIXEL_7X7, 		// dot pixel 7 X 7
	DOT_PIXEL_8X8, 		// dot pixel 8 X 8
} DOT_PIXEL;
#define DOT_PIXEL_DFT  DOT_PIXEL_1X1  //Default dot pilex

/********************************************************************************
function:
			dot Fill style
********************************************************************************/
typedef enum {
	DOT_FILL_AROUND = 1,		// dot pixel 1 x 1
	DOT_FILL_RIGHTUP, 		// dot pixel 2 X 2
} DOT_STYLE;
#define DOT_STYLE_DFT  DOT_FILL_AROUND  //Default dot pilex
/********************************************************************************
function:
			solid line and dotted line
********************************************************************************/
typedef enum {
	LINE_SOLID = 0,
	LINE_DOTTED,
} LINE_STYLE;

/********************************************************************************
function:
			DRAW Internal fill
********************************************************************************/
typedef enum {
	DRAW_EMPTY = 0,
	DRAW_FULL,
} DRAW_FILL;

//Touch screen structure
typedef struct {
	POINT Xpoint0;
	POINT Ypoint0;
	POINT Xpoint;
	POINT Ypoint;
	uint8_t chStatus;
	uint8_t chType;
	int16_t iXoff;
	int16_t iYoff;
	float fXfac;
	float fYfac;
	//Select the coordinates of the XPT2046 touch \
	  screen relative to what scan direction
	LCD_SCAN_DIR TP_Scan_Dir;
}TP_DEV;

//Brush structure
typedef struct{
	POINT Xpoint;
	POINT Ypoint;
	COLOR Color;
	DOT_PIXEL DotPixel; 
}TP_DRAW;

class cTouch : public MMC_SD{
public:
	cTouch() {};
//	cTouch(Adafruit_SPITFT *tft);
	void GetAdFac(void);
	void Adjust(void);
	void Init(LCD_SCAN_DIR Lcd_ScanDir, Adafruit_SPITFT *mytft);
	uint8_t Scan(uint8_t chCoordType);
	TP_DEV status() { return sTP_DEV; }
	TP_DRAW DrawPoint() { return sTP_Draw; }
protected:
	TP_DEV sTP_DEV;
	TP_DRAW sTP_Draw;
	Adafruit_SPITFT* tft;
	//	void GetAdFac(void);
	void DrawCross(POINT Xpoint, POINT Ypoint, COLOR Color);
	bool Read_TwiceADC(uint16_t* pXCh_Adc, uint16_t* pYCh_Adc);
	void Read_ADC_XY(uint16_t* pXCh_Adc, uint16_t* pYCh_Adc);
	uint16_t Read_ADC_Average(uint8_t Channel_Cmd);
	uint16_t Read_ADC(uint8_t CMD);
	void ShowInfo(POINT Xpoint0, POINT Ypoint0,
		POINT Xpoint1, POINT Ypoint1,
		POINT Xpoint2, POINT Ypoint2,
		POINT Xpoint3, POINT Ypoint3,
		POINT hwFac);
};
#endif
