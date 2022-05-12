/*
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
#include "Adafruit_GFX.h"
#include "touch.h"
#include "f_util.h"
#include "ff.h"
#include "hw_config.h"

extern const GFXfont FreeSans9pt7b;
extern const GFXfont FreeSans12pt7b;
extern const GFXfont FreeSans18pt7b;
extern const GFXfont FreeSans24pt7b;

extern cTouch TP;
extern Adafruit_SPITFT tft;
//extern ImageReader reader;
int main(void);
void hid_task(void);
