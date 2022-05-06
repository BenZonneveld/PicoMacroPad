#include "GFX-lib/Adafruit_GFX.h"
#include "GFX-lib/Adafruit_SPITFT.h"
#include "mmc_sd/MMC_SD.h"
#include "TFTSDTouch.h"

TFTSDTouch::TFTSDTouch()
{
	tft = Adafruit_SPITFT();
	mmc = MMC_SD();
}