#include "Adafruit_GFX.h"
#include "Adafruit_SPITFT.h"
#include "MMC_SD.h"

class TFTSDTouch{
public:
	Adafruit_SPITFT tft;
	MMC_SD			mmc;
//	tft.init();
	TFTSDTouch();// { tft = TFTSDTouch::Adafruit_SPITFT(); }
//	MMC_SD			mmc;
//	TFTSDTouch		tp;
};