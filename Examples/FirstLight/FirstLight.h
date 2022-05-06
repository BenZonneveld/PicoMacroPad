/*
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

 // For the breakout board, you can use any 2 or 3 pins.
 // These pins will also work for the 1.8" TFT shield.
//#define TFT_CS         9
//#define TFT_RST       15 // Or set to -1 and connect to Arduino RESET pin
//#define TFT_DC         8

#define LCD_RST_PIN		15	
#define LCD_DC_PIN		8
#define LCD_CS_PIN		9
#define LCD_CLK_PIN		10
#define LCD_BKL_PIN		13
#define LCD_MOSI_PIN	11
#define LCD_MISO_PIN	12
#define TP_CS_PIN		16
#define TP_IRQ_PIN		17
#define SD_CS_PIN		22

#define SPI_PORT		spi1

int main(void);
void loop();
void testlines(uint16_t color);
void testdrawtext(char* text, uint16_t color);
void testfastlines(uint16_t color1, uint16_t color2);
void testdrawrects(uint16_t color);
void testfillrects(uint16_t color1, uint16_t color2);
void testfillcircles(uint8_t radius, uint16_t color);
void testdrawcircles(uint8_t radius, uint16_t color);
void testtriangles();
void testroundrects();
void tftPrintTest();
void mediabuttons();
