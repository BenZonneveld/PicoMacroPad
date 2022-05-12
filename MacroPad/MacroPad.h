/*
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
#include "Adafruit_GFX.h"
#include "touch.h"

extern const GFXfont FreeSans9pt7b;
extern const GFXfont FreeSans12pt7b;
extern const GFXfont FreeSans18pt7b;
extern const GFXfont FreeSans24pt7b;

extern cTouch TP;
extern Adafruit_SPITFT tft;

#define PIN_PAGE_NEXT 20
#define PIN_PAGE_PREV 21

int main(void);
void hid_task(void);
void inter_test(uint gpio, uint32_t events);
