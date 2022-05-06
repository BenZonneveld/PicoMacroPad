// TFT_ Display.cpp : Defines the entry point for the application.
//

#include <stdlib.h>
#include <stdio.h>
#include <pico/stdlib.h>
#include <hardware/spi.h>

#include "FirstLight.h"

#include "Adafruit_GFX.h"    // Core graphics library
#include "Adafruit_SPITFT.h" // Hardware-specific library for ST7789
#include "TFTSDTouch.h"

//Adafruit_SPITFT tft = Adafruit_SPITFT(240,320);
TFTSDTouch device = TFTSDTouch();

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

    device.tft.init(240, 320);           // Init ST7789 320x240

    // SPI speed defaults to SPI_DEFAULT_FREQ defined in the library, you can override it here
    // Note that speed allowable depends on chip and quality of wiring, if you go too fast, you
    // may end up with a black screen some times, or all the time.
    //tft.setSPISpeed(80000000);

    printf("Initialized\r\n");

    uint32_t time = millis();
    device.tft.fillScreen(ST77XX_BLACK);
    time = millis() - time;

    printf("%i to fill screen \r\n",time);
    sleep_ms(500);
    while (1)
    {

        // large block of text
        device.tft.fillScreen(ST77XX_BLACK);
        time = millis();
        testdrawtext((char*)"Lorem ipsum dolor sit amet, consectetur adipiscing elit. Curabitur adipiscing ante sed nibh tincidunt feugiat. Maecenas enim massa, fringilla sed malesuada et, malesuada sit amet turpis. Sed porttitor neque ut ante pretium vitae malesuada nunc bibendum. Nullam aliquet ultrices massa eu hendrerit. Ut sed nisi lorem. In vestibulum purus a tortor imperdiet posuere. ", ST77XX_WHITE);
        time = millis() - time;
        printf("%i to draw Text \r\n", time);
        sleep_ms(1000);

        // tft print function!
        time = millis();
        tftPrintTest();
        time = millis() - time;
        printf("%i to Print Test \r\n", time);
        sleep_ms(4000);

        // a single pixel
        time = millis();
        device.tft.drawPixel(device.tft.width() / 2, device.tft.height() / 2, ST77XX_GREEN);
        time = millis() - time;
        printf("%i to draw Pixel\r\n", time);

        sleep_ms(500);

        // line draw test
        time = millis();
        testlines(ST77XX_YELLOW);
        time = millis() - time;
        printf("%i lines \r\n", time);

        sleep_ms(500);

        // optimized lines
        time = millis();
        testfastlines(ST77XX_RED, ST77XX_BLUE);
        time = millis() - time;
        printf("%i to do fastlines \r\n", time);

        sleep_ms(500);

        time = millis();
        testdrawrects(ST77XX_GREEN);
        time = millis() - time;
        printf("%i to drawrects \r\n", time);

        sleep_ms(500);

        time = millis();
        testfillrects(ST77XX_YELLOW, ST77XX_MAGENTA);
        time = millis() - time;
        printf("%i to do filledrects \r\n", time);

        sleep_ms(500);

        device.tft.fillScreen(ST77XX_BLACK);
        time = millis();
        testfillcircles(10, ST77XX_BLUE);
        time = millis() - time;
        printf("%i for fill circles \r\n", time);

        time = millis();
        testdrawcircles(10, ST77XX_WHITE);
        time = millis() - time;
        printf("%i to draw Circles \r\n", time);

        sleep_ms(500);

        time = millis();
        testroundrects();
        time = millis() - time;
        printf("%i for round rects\r\n", time);

        sleep_ms(500);

        time = millis();
        testtriangles();
        time = millis() - time;
        printf("%i for triangles\r\n", time);

        sleep_ms(500);

        time = millis();
        mediabuttons();
        time = millis() - time;
        printf("%i for mediabuttons\r\n", time);

        sleep_ms(500);

        printf("done\r\n");
        sleep_ms(1000);
    }
    return 0;
}

void testlines(uint16_t color) {
    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < device.tft.width(); x += 6) {
        device.tft.drawLine(0, 0, x, device.tft.height() - 1, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < device.tft.height(); y += 6) {
        device.tft.drawLine(0, 0, device.tft.width() - 1, y, color);
        sleep_ms(0);
    }

    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < device.tft.width(); x += 6) {
        device.tft.drawLine(device.tft.width() - 1, 0, x, device.tft.height() - 1, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < device.tft.height(); y += 6) {
        device.tft.drawLine(device.tft.width() - 1, 0, 0, y, color);
        sleep_ms(0);
    }

    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < device.tft.width(); x += 6) {
        device.tft.drawLine(0, device.tft.height() - 1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < device.tft.height(); y += 6) {
        device.tft.drawLine(0, device.tft.height() - 1, device.tft.width() - 1, y, color);
        sleep_ms(0);
    }

    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < device.tft.width(); x += 6) {
        device.tft.drawLine(device.tft.width() - 1, device.tft.height() - 1, x, 0, color);
        sleep_ms(0);
    }
    for (int16_t y = 0; y < device.tft.height(); y += 6) {
        device.tft.drawLine(device.tft.width() - 1, device.tft.height() - 1, 0, y, color);
        sleep_ms(0);
    }
}

void testdrawtext(char* text, uint16_t color) {
    device.tft.setCursor(0, 0);
    device.tft.setTextColor(color);
    device.tft.setTextWrap(true);
    device.tft.print(text);
}

void testfastlines(uint16_t color1, uint16_t color2) {
    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t y = 0; y < device.tft.height(); y += 5) {
        device.tft.drawFastHLine(0, y, device.tft.width(), color1);
    }
    for (int16_t x = 0; x < device.tft.width(); x += 5) {
        device.tft.drawFastVLine(x, 0, device.tft.height(), color2);
    }
}

void testdrawrects(uint16_t color) {
    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = 0; x < device.tft.width(); x += 6) {
        device.tft.drawRect(device.tft.width() / 2 - x / 2, device.tft.height() / 2 - x / 2, x, x, color);
    }
}

void testfillrects(uint16_t color1, uint16_t color2) {
    device.tft.fillScreen(ST77XX_BLACK);
    for (int16_t x = device.tft.width() - 1; x > 6; x -= 6) {
        device.tft.fillRect(device.tft.width() / 2 - x / 2, device.tft.height() / 2 - x / 2, x, x, color1);
        device.tft.drawRect(device.tft.width() / 2 - x / 2, device.tft.height() / 2 - x / 2, x, x, color2);
    }
}

void testfillcircles(uint8_t radius, uint16_t color) {
    for (int16_t x = radius; x < device.tft.width(); x += radius * 2) {
        for (int16_t y = radius; y < device.tft.height(); y += radius * 2) {
            device.tft.fillCircle(x, y, radius, color);
        }
    }
}

void testdrawcircles(uint8_t radius, uint16_t color) {
    for (int16_t x = 0; x < device.tft.width() + radius; x += radius * 2) {
        for (int16_t y = 0; y < device.tft.height() + radius; y += radius * 2) {
            device.tft.drawCircle(x, y, radius, color);
        }
    }
}

void testtriangles() {
    device.tft.fillScreen(ST77XX_BLACK);
    uint16_t color = 0xF800;
    int t;
    int w = device.tft.width() / 2;
    int x = device.tft.height() - 1;
    int y = 0;
    int z = device.tft.width();
    for (t = 0; t <= 15; t++) {
        device.tft.drawTriangle(w, y, y, x, z, x, color);
        x -= 4;
        y += 4;
        z -= 4;
        color += 100;
    }
}

void testroundrects() {
    device.tft.fillScreen(ST77XX_BLACK);
    uint16_t color = 100;
    int i;
    int t;
    for (t = 0; t <= 4; t += 1) {
        int x = 0;
        int y = 0;
        int w = device.tft.width() - 2;
        int h = device.tft.height() - 2;
        for (i = 0; i <= 16; i += 1) {
            device.tft.drawRoundRect(x, y, w, h, 5, color);
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
    device.tft.setTextWrap(false);
    device.tft.fillScreen(ST77XX_BLACK);
    device.tft.setCursor(0, 30);
    device.tft.setTextColor(ST77XX_RED);
    device.tft.setTextSize(1);
    device.tft.println("Hello World!");
    device.tft.setTextColor(ST77XX_YELLOW);
    device.tft.setTextSize(2);
    device.tft.println("Hello World!");
    device.tft.setTextColor(ST77XX_GREEN);
    device.tft.setTextSize(3);
    device.tft.println("Hello World!");
    device.tft.setTextColor(ST77XX_BLUE);
    device.tft.setTextSize(4);
    device.tft.print(1234.567);
    sleep_ms(1500);
    device.tft.setCursor(0, 0);
    device.tft.fillScreen(ST77XX_BLACK);
    device.tft.setTextColor(ST77XX_WHITE);
    device.tft.setTextSize(0);
    device.tft.println("Hello World!");
    device.tft.setTextSize(1);
    device.tft.setTextColor(ST77XX_GREEN);
    device.tft.print(p, 6);
    device.tft.println(" Want pi?");
    device.tft.println(" ");
    device.tft.print(8675309, HEX); // print 8,675,309 out in HEX!
    device.tft.println(" Print HEX!");
    device.tft.println(" ");
    device.tft.setTextColor(ST77XX_WHITE);
    device.tft.println("Sketch has been");
    device.tft.println("running for: ");
    device.tft.setTextColor(ST77XX_MAGENTA);
    device.tft.print(millis() / 1000);
    device.tft.setTextColor(ST77XX_WHITE);
    device.tft.print(" seconds.");
}

void mediabuttons() {
    // play
    device.tft.fillScreen(ST77XX_BLACK);
    device.tft.fillRoundRect(25, 10, 78, 60, 8, ST77XX_WHITE);
    device.tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_RED);
    sleep_ms(500);
    // pause
    device.tft.fillRoundRect(25, 90, 78, 60, 8, ST77XX_WHITE);
    device.tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_GREEN);
    device.tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_GREEN);
    sleep_ms(500);
    // play color
    device.tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_BLUE);
    sleep_ms(50);
    // pause color
    device.tft.fillRoundRect(39, 98, 20, 45, 5, ST77XX_RED);
    device.tft.fillRoundRect(69, 98, 20, 45, 5, ST77XX_RED);
    // play color
    device.tft.fillTriangle(42, 20, 42, 60, 90, 40, ST77XX_GREEN);
}
