/*
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/
#include "Adafruit_GFX.h"
#include "touch.h"
#include "debounce.h"
#include "MacroHandler.h"
#include "gui.h"
#include "midi.h"

extern const GFXfont FreeSans9pt7b;
extern const GFXfont FreeSans12pt7b;
extern const GFXfont FreeSans18pt7b;
extern const GFXfont FreeSans24pt7b;

extern cGUI gui;
extern cTouch TP;
extern Adafruit_SPITFT tft;
extern Debounce debouncer;
extern CMacro h_macro;

#define MAXIDLE 30
#define MAXFADE 96
#define PIN_SB3 2
#define PIN_SB2	3
#define PIN_SB1	4
#define PIN_SB0	5
#define SB0 0x1
#define SB1 0x2
#define SB2 0x4
#define SB3 0x8
// uint64_t us_time;
int main(void);
void taskloop();
void doSoftButtons();
void gpio_callback(uint gpio, uint32_t events);
void disableSleep();
