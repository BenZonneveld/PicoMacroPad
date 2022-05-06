// TFT_ Display.cpp : Defines the entry point for the application.
//

#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>

#include "FirstLight.h"

#include <Adafruit_GFX.h>    // Core graphics library
//#include <Adafruit_ST7735.h> // Hardware-specific library for ST7735
#include <Adafruit_SPITFT.h> // Hardware-specific library for ST7789

// OPTION 1 (recommended) is to use the HARDWARE SPI pins, which are unique
// to each board and not reassignable. For Arduino Uno: MOSI = pin 11 and
// SCLK = pin 13. This is the fastest mode of operation and is required if
// using the breakout board's microSD card.

// For 1.44" and 1.8" TFT with ST7735 use:
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_RST);

// For 1.14", 1.3", 1.54", 1.69", and 2.0" TFT with ST7789:
Adafruit_SPITFT tft = Adafruit_SPITFT(240,320);


// OPTION 2 lets you interface the display using ANY TWO or THREE PINS,
// tradeoff being that performance is not as fast as hardware SPI above.
//#define TFT_MOSI 11  // Data out
//#define TFT_SCLK 13  // Clock out

// For ST7735-based displays, we will use this call
//Adafruit_ST7735 tft = Adafruit_ST7735(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);

// OR for the ST7789-based displays, we will use this call
//Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_MOSI, TFT_SCLK, TFT_RST);


float p = 3.1415926;

uint32_t millis()
{
    return to_ms_since_boot(get_absolute_time());
}

int main(void) {

    stdio_init_all();
//    Serial.begin(9600);
    sleep_ms(5000);
    printf("Hello! ST77xx TFT Test\r\n");

    tft.init(240, 320);           // Init ST7789 320x240

    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    //tft.setSPISpeed(80000000);

    printf("Initialized\r\n");

    uint16_t time = millis();
    tft.fillScreen(ST77XX_BLACK);
    time = millis() - time;

    printf("%i\r\n",time);
    sleep_ms(500);

    // large block of text
    tft.fillScreen(ST77XX_BLACK);
    testdrawtext((char *)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
    sleep_ms(1000);

    // tft print function!
    tftPrintTest();
    sleep_ms(4000);

    // a single pixel
    tft.drawPixel(tft.width() / 2, tft.height() / 2, ST77XX_GREEN);
    sleep_ms(500);

    // line draw test
    testlines(ST77XX_YELLOW);
    sleep_ms(500);

    // optimized lines
    testfastlines(ST77XX_RED, ST77XX_BLUE);
    sleep_ms(500);

    testdrawrects(ST77XX_GREEN);
    sleep_ms(500);

    testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
    sleep_ms(500);

    tft.fillScreen(ST77XX_BLACK);
    testfillcircles(10, ST77XX_BLUE);
    testdrawcircles(10, ST77XX_WHITE);
    sleep_ms(500);

    testroundrects();
    sleep_ms(500);

    testtriangles();
    sleep_ms(500);

    mediabuttons();
    sleep_ms(500);

    printf("done\r\n");
    sleep_ms(1000);
    while (1)
    {
        loop();
    }
    return 0;
}

void loop() {
    tft.invertDisplay(true);
    sleep_ms(500);
    tft.invertDisplay(false);
    sleep_ms(500);
}

void testlines(uint16_t color) {
    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < tft.width(); x += 6) {
        tft.drawLine(0, 0, x, tft.height() - 1, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < tft.height(); y += 6) {
        tft.drawLine(0, 0, tft.width() - 1, y, color);
        sleep_ms(0);
    }

    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < tft.width(); x += 6) {
        tft.drawLine(tft.width() - 1, 0, x, tft.height() - 1, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < tft.height(); y += 6) {
        tft.drawLine(tft.width() - 1, 0, 0, y, color);
        sleep_ms(0);
    }

    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < tft.width(); x += 6) {
        tft.drawLine(0, tft.height() - 1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < tft.height(); y += 6) {
        tft.drawLine(0, tft.height() - 1, tft.width() - 1, y, color);
        sleep_ms(0);
    }

    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < tft.width(); x += 6) {
        tft.drawLine(tft.width() - 1, tft.height() - 1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < tft.height(); y += 6) {
        tft.drawLine(tft.width() - 1, tft.height() - 1, 0, y, color);
        sleep_ms(0);
    }
}

void testdrawtext(char* text, uint16_t color) {
    tft.setCursor(0, 0);
    tft.setTextColor(color);
    tft.setTextWrap(true);
    tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
    tft.fillScreen(ST77XX_BLACK);
    for (int16_t y = 0; y < tft.height(); y += 5) {
        tft.drawFastHLine(0, y, tft.width(), color1);
    }
    for (int16_t x = 0; x < tft.width(); x += 5) {
        tft.drawFastVLine(x, 0, tft.height(), color2);
    }
}

void testdrawrects(uint16_t color) {
    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < tft.width(); x += 6) {
        tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color);
    }
}

void testfillrects(uint16_t color1, uint16_t color2) {
    tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = tft.width() - 1; x > 6; x -= 6) {
        tft.fillRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color1);
        tft.drawRect(tft.width() / 2 - x / 2, tft.height() / 2 - x / 2, x, x, color2);
    }
}

void testfillcircles(uint8_t radius, uint16_t color) {
    for (int16_t x = radius; x < tft.width(); x += radius * 2) {
        for (int16_t y = radius; y < tft.height(); y += radius * 2) {
            tft.fillCircle(x, y, radius, color);
        }
    }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
    for (int16_t x = 0; x < tft.width() + radius; x += radius * 2) {
        for (int16_t y = 0; y < tft.height() + radius; y += radius * 2) {
            tft.drawCircle(x, y, radius, color);
        }
    }
}

void testtriangles() {
    tft.fillScreen(ST77XX_BLACK);
    uint16_t color = 0xF800;
    int t;
    int w = tft.width() / 2;
    int x = tft.height() - 1;
    int y = 0;
    int z = tft.width();
    for (t = 0; t <= 15; t++) {
        tft.drawTriangle(w, y, y, x, z, x, color);
        x -= 4;
        y += 4;
        z -= 4;
        color += 100;
    }
}

void testroundrects() {
    tft.fillScreen(ST77XX_BLACK);
    uint16_t color = 100;
    int i;
    int t;
    for (t = 0; t <= 4; t += 1) {
        int x = 0;
        int y = 0;
        int w = tft.width() - 2;
        int h = tft.height() - 2;
        for (i = 0; i <= 16; i += 1) {
            tft.drawRoundRect(x, y, w, h, 5, color);
            x += 2;
            y += 3;
            w -= 4;
            h -= 6;
            color += 1100;
        }
        color += 100;
    }
}

void tftPrintTest() {
    tft.setTextWrap(false);
    tft.fillScreen(ST77XX_BLACK);
    tft.setCursor(0, 30);
    tft.setTextColor(ST77XX_RED);
    tft.setTextSize(1);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_YELLOW);
    tft.setTextSize(2);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_GREEN);
    tft.setTextSize(3);
    tft.println("Hello World!");
    tft.setTextColor(ST77XX_BLUE);
    tft.setTextSize(4);
    tft.print(1234.567);
    sleep_ms(1500);
    tft.setCursor(0, 0);
    tft.fillScreen(ST77XX_BLACK);
    tft.setTextColor(ST77XX_WHITE);
    tft.setTextSize(0);
    tft.println("Hello World!");
    tft.setTextSize(1);
    tft.setTextColor(ST77XX_GREEN);
    tft.print(p, 6);
    tft.println(" Want pi?");
    tft.println(" ");
    tft.print(8675309, HEX); // print 8,675,309 out in HEX!
    tft.println(" Print HEX!");
    tft.println(" ");
    tft.setTextColor(ST77XX_WHITE);
    tft.println("Sketch has been");
    tft.println("running for: ");
    tft.setTextColor(ST77XX_MAGENTA);
    tft.print(millis() / 1000);
    tft.setTextColor(ST77XX_WHITE);
    tft.print(" seconds.");
}

void mediabuttons() {
    // play
    tft.fillScreen(ST77XX_BLACK);
    tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
    tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
    sleep_ms(500);
    // pause
    tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
    tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
    tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
    sleep_ms(500);
    // play color
    tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
    sleep_ms(50);
    // pause color
    tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
    tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
    // play color
    tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
