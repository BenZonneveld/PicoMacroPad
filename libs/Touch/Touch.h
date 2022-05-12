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
#include "Adafruit_SPITFT.h"

#define	COLOR				uint16_t		//The variable type of the color (unsigned short) 
#define	POINT				uint16_t		//The type of coordinate (unsigned short) 
#define	LENGTH				uint16_t		//The type of coordinate (unsigned short) 

#define SPI_TP_FREQ (3 * 1000000) ///< Default SPI data clock frequency

#define TP_PRESS_DOWN           0x80
#define TP_PRESSED              0x40

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
}TP_DEV;

//Brush structure
typedef struct{
	POINT Xpoint;
	POINT Ypoint;
}TP_DRAW;

class cTouch {
public:
	cTouch() {};
//	cTouch(Adafruit_SPITFT *tft);
	void GetAdFac(void);
	void Adjust(void);
	void Init(Adafruit_SPITFT *mytft);
	uint8_t Scan(bool chCoordType = false);
	TP_DEV status() { return sTP_DEV; }
	TP_DRAW DrawPoint() { return sTP_Draw; }
protected:
	TP_DEV sTP_DEV;
	TP_DRAW sTP_Draw;
	Adafruit_SPITFT* tft;
	//	void GetAdFac(void);
	uint8_t SPI4W_Write_Byte(uint8_t value);
	uint8_t SPI4W_Read_Byte(uint8_t value);
	void DrawCross(POINT Xpoint, POINT Ypoint, COLOR Color);
	bool Read_TwiceADC(uint16_t* pXCh_Adc, uint16_t* pYCh_Adc);
	void Read_ADC_XY(uint16_t* pXCh_Adc, uint16_t* pYCh_Adc);
	uint16_t Read_ADC_Average(uint8_t Channel_Cmd);
	uint16_t Read_ADC(uint8_t CMD);
};
#endif
