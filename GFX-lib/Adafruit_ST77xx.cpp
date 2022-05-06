/**************************************************************************
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include "Adafruit_SPITFT.h"
#include <limits.h>
#include "hardware/spi.h"


/**************************************************************************/

/**************************************************************************/
/*!
    @brief  Companion code to the initiliazation tables. Reads and issues
            a series of LCD commands stored in PROGMEM byte array.
    @param  addr  Flash memory array with commands and data to send
*/
/**************************************************************************/
void Adafruit_SPITFT::displayInit(const uint8_t *addr) {

  uint8_t numCommands, cmd, numArgs;
  uint16_t ms;

  gpio_init(LCD_BKL_PIN);
  gpio_set_dir(LCD_BKL_PIN, GPIO_OUT);
  gpio_put(LCD_BKL_PIN, 1);

  numCommands = *(addr++); // Number of commands to follow
  while (numCommands--) {              // For each command...
    cmd = *(addr++);       // Read command
    numArgs = *(addr++);   // Number of args to follow
    ms = numArgs & ST_CMD_DELAY;       // If hibit set, delay follows args
    numArgs &= ~ST_CMD_DELAY;          // Mask out delay bit
    printf("cmd: %.2X numArgs: %i\r\n", cmd, numArgs);
    sendCommand(cmd, addr, numArgs);
    addr += numArgs;

    if (ms) {
      ms = *(addr++); // Read post-command delay time (ms)
      if (ms == 255)
        ms = 500; // If 255, delay for 500 ms
      printf("Delay: %i ms\r\n", ms);
      sleep_ms(ms);
    }
  }
}

/**************************************************************************/
/*!
    @brief  Initialize ST77xx chip. Connects to the ST77XX over SPI and
            sends initialization procedure commands
    @param  freq  Desired SPI clock frequency
*/
/**************************************************************************/
void Adafruit_SPITFT::begin(uint32_t freq) {
  if (!freq) {
    freq = SPI_DEFAULT_FREQ;
  }
  _freq = freq;

  invertOnCommand = ST77XX_INVON;
  invertOffCommand = ST77XX_INVOFF;

  // LCD Reset
  gpio_init(LCD_RST_PIN);
  gpio_set_dir(LCD_RST_PIN, GPIO_OUT);
  gpio_put(LCD_RST_PIN, 1);
  sleep_ms(500);
  gpio_put(LCD_RST_PIN, 0);
  sleep_ms(500);
  gpio_put(LCD_RST_PIN, 1);
  sleep_ms(500);

  initSPI();

}

/**************************************************************************/
/*!
    @brief  Initialization code common to all ST77XX displays
    @param  cmdList  Flash memory array with commands and data to send
*/
/**************************************************************************/
void Adafruit_SPITFT::commonInit(const uint8_t *cmdList) {
  begin();

  if (cmdList) {
    displayInit(cmdList);
  }
}

/**************************************************************************/
/*!
  @brief  SPI displays set an address window rectangle for blitting pixels
  @param  x  Top left corner x coordinate
  @param  y  Top left corner x coordinate
  @param  w  Width of window
  @param  h  Height of window
*/
/**************************************************************************/
void Adafruit_SPITFT::setAddrWindow(uint16_t x, uint16_t y, uint16_t w,
                                    uint16_t h) {
  x += _xstart;
  y += _ystart;
  uint32_t xa = ((uint32_t)x << 16) | (x + w - 1);
  uint32_t ya = ((uint32_t)y << 16) | (y + h - 1);

  writeCommand(ST77XX_CASET); // Column addr set
  SPI_WRITE32(xa);

  writeCommand(ST77XX_RASET); // Row addr set
  SPI_WRITE32(ya);

  writeCommand(ST77XX_RAMWR); // write to RAM
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
/*void Adafruit_SPITFT::setRotation(uint8_t m) {
  uint8_t madctl = 0;

  rotation = m % 4; // can't be higher than 3

  switch (rotation) {
  case 0:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 1:
    madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  case 2:
    madctl = ST77XX_MADCTL_RGB;
    _xstart = _colstart;
    _ystart = _rowstart;
    break;
  case 3:
    madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
    _ystart = _colstart;
    _xstart = _rowstart;
    break;
  }

  sendCommand(ST77XX_MADCTL, &madctl, 1);
}
*/
/**************************************************************************/
/*!
    @brief  Set origin of (0,0) of display with offsets
    @param  col  The offset from 0 for the column address
    @param  row  The offset from 0 for the row address
*/
/**************************************************************************/
void Adafruit_SPITFT::setColRowStart(int8_t col, int8_t row) {
  _colstart = col;
  _rowstart = row;
}

/**************************************************************************/
/*!
 @brief  Change whether display is on or off
 @param  enable True if you want the display ON, false OFF
 */
/**************************************************************************/
void Adafruit_SPITFT::enableDisplay(bool enable) {
  sendCommand(enable ? ST77XX_DISPON : ST77XX_DISPOFF);
}

/**************************************************************************/
/*!
 @brief  Change whether TE pin output is on or off
 @param  enable True if you want the TE pin ON, false OFF
 */
/**************************************************************************/
void Adafruit_SPITFT::enableTearing(bool enable) {
  sendCommand(enable ? ST77XX_TEON : ST77XX_TEOFF);
}

/**************************************************************************/
/*!
 @brief  Change whether sleep mode is on or off
 @param  enable True if you want sleep mode ON, false OFF
 */
/**************************************************************************/
void Adafruit_SPITFT::enableSleep(bool enable) {
  sendCommand(enable ? ST77XX_SLPIN : ST77XX_SLPOUT);
}

////////// stuff not actively being used, but kept for posterity
/*

 uint8_t Adafruit_ST77xx::spiread(void) {
 uint8_t r = 0;
 if (_sid > 0) {
 r = shiftIn(_sid, _sclk, MSBFIRST);
 } else {
 //SID_DDR &= ~_BV(SID);
 //int8_t i;
 //for (i=7; i>=0; i--) {
 //  SCLK_PORT &= ~_BV(SCLK);
 //  r <<= 1;
 //  r |= (SID_PIN >> SID) & 0x1;
 //  SCLK_PORT |= _BV(SCLK);
 //}
 //SID_DDR |= _BV(SID);

 }
 return r;
 }

 void Adafruit_ST77xx::dummyclock(void) {

 if (_sid > 0) {
 digitalWrite(_sclk, LOW);
 digitalWrite(_sclk, HIGH);
 } else {
 // SCLK_PORT &= ~_BV(SCLK);
 //SCLK_PORT |= _BV(SCLK);
 }
 }
 uint8_t Adafruit_ST77xx::readdata(void) {
 *portOutputRegister(rsport) |= rspin;

 *portOutputRegister(csport) &= ~ cspin;

 uint8_t r = spiread();

 *portOutputRegister(csport) |= cspin;

 return r;

 }

 uint8_t Adafruit_ST77xx::readcommand8(uint8_t c) {
 digitalWrite(_rs, LOW);

 *portOutputRegister(csport) &= ~ cspin;

 spiwrite(c);

 digitalWrite(_rs, HIGH);
 pinMode(_sid, INPUT); // input!
 digitalWrite(_sid, LOW); // low
 spiread();
 uint8_t r = spiread();


 *portOutputRegister(csport) |= cspin;


 pinMode(_sid, OUTPUT); // back to output
 return r;
 }


 uint16_t Adafruit_ST77xx::readcommand16(uint8_t c) {
 digitalWrite(_rs, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);

 spiwrite(c);
 pinMode(_sid, INPUT); // input!
 uint16_t r = spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);

 pinMode(_sid, OUTPUT); // back to output
 return r;
 }

 uint32_t Adafruit_ST77xx::readcommand32(uint8_t c) {
 digitalWrite(_rs, LOW);
 if (_cs)
 digitalWrite(_cs, LOW);
 spiwrite(c);
 pinMode(_sid, INPUT); // input!

 dummyclock();
 dummyclock();

 uint32_t r = spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 r <<= 8;
 r |= spiread();
 if (_cs)
 digitalWrite(_cs, HIGH);

 pinMode(_sid, OUTPUT); // back to output
 return r;
 }

 */

 // CONSTRUCTORS ************************************************************

 /*!
     @brief  Instantiate Adafruit ST7789 driver
 */
//Adafruit_ST7789::Adafruit_ST7789()
//    : Adafruit_ST77xx(240, 320) {}

// SCREEN INITIALIZATION ***************************************************

// Rather than a bazillion writecommand() and writedata() calls, screen
// initialization commands and arguments are organized in these tables
// stored in PROGMEM.  The table may look bulky, but that's mostly the
// formatting -- storage-wise this is hundreds of bytes more compact
// than the equivalent code.  Companion function follows.

// clang-format off

static const uint8_t st7789[] = {                // Init commands for 7789 screens
    16,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      100,                          //     ~150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      10,                          //      10 ms delay
    ST77XX_COLMOD , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    PORCTRL, 5 + ST_CMD_DELAY,
     0x0c,
     0x0c,
     0x00,
     0x33,
     0x33,
     10,
    GCTRL , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x35,                         //     16-bit color
      10,                           //     10 ms delay
    VCOMS , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x28,                         //     16-bit color
      10,                           //     10 ms delay
    LCMCTRL , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x3c,                         //     16-bit color
      10,                           //     10 ms delay
    VDVVRHEN , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x01,                         //     16-bit color
      10,                           //     10 ms delay
    VRHS , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x0b,                         //     16-bit color
      10,                           //     10 ms delay
    VDVS , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x20,                         //     16-bit color
      10,                           //     10 ms delay
    FRCTRL2 , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x0f,                         //     16-bit color
      10,                           //     10 ms delay
    PWCTRL1 , 2 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0xa4,                         //     16-bit color
      0xa1,
      10,                           //     10 ms delay
    PVGAMCTRL , 14 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0xd0,
      0x01,
      0x08,
      0x0f,
      0x11,
      0x2a,
      0x36,
      0x55,
      0x44,
      0x3a,
      0x0b,
      0x06,
      0x11,
      0x20,
      10,                           //     10 ms delay
    NVGAMCTRL, 14 + ST_CMD_DELAY,
    0xd0,
    0x02,
    0x07,
    0x0a,
    0x0b,
    0x18,
    0x34,
    0x43,
    0x4a,
    0x2b,
    0x1b,
    0x1c,
    0x22,
    0x1f,
    10,
    WRCACE, 1 + ST_CMD_DELAY,
    0xB0,
    10,
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
      10
};                          //    10 ms delay

// clang-format on

/**************************************************************************/
/*!
    @brief  Initialization code common to all ST7789 displays
    @param  width  Display width
    @param  height Display height
    @param  mode   SPI data mode; one of SPI_MODE0, SPI_MODE1, SPI_MODE2
                   or SPI_MODE3 (do NOT pass the numbers 0,1,2 or 3 -- use
                   the defines only, the values are NOT the same!)
*/
/**************************************************************************/
void Adafruit_SPITFT::init(uint16_t width, uint16_t height) {
    commonInit(NULL);
    if (width == 240 && height == 240) {
        // 1.3", 1.54" displays (right justified)
        _rowstart = (320 - height);
        _rowstart2 = 0;
        _colstart = _colstart2 = (240 - width);
    }
    else if (width == 135 && height == 240) {
        // 1.14" display (centered, with odd size)
        _rowstart = _rowstart2 = (int)((320 - height) / 2);
        // This is the only device currently supported device that has different
        // values for _colstart & _colstart2. You must ensure that the extra
        // pixel lands in _colstart and not in _colstart2
        _colstart = (int)((240 - width + 1) / 2);
        _colstart2 = (int)((240 - width) / 2);
    }
    else {
        // 1.47", 1.69, 1.9", 2.0" displays (centered)
        _rowstart = _rowstart2 = (int)((320 - height) / 2);
        _colstart = _colstart2 = (int)((240 - width) / 2);
    }

    windowWidth = width;
    windowHeight = height;

    displayInit(st7789);
    setRotation(0);
}

/**************************************************************************/
/*!
    @brief  Set origin of (0,0) and orientation of TFT display
    @param  m  The index for rotation, from 0-3 inclusive
*/
/**************************************************************************/
void Adafruit_SPITFT::setRotation(uint8_t m) {
    uint8_t madctl = 0;

    rotation = m & 3; // can't be higher than 3

    switch (rotation) {
    case 0:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MY | ST77XX_MADCTL_RGB;
        _xstart = _colstart;
        _ystart = _rowstart;
        _width = windowWidth;
        _height = windowHeight;
        break;
    case 1:
        madctl = ST77XX_MADCTL_MY | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        _xstart = _rowstart;
        _ystart = _colstart2;
        _height = windowWidth;
        _width = windowHeight;
        break;
    case 2:
        madctl = ST77XX_MADCTL_RGB;
        _xstart = _colstart2;
        _ystart = _rowstart2;
        _width = windowWidth;
        _height = windowHeight;
        break;
    case 3:
        madctl = ST77XX_MADCTL_MX | ST77XX_MADCTL_MV | ST77XX_MADCTL_RGB;
        _xstart = _rowstart2;
        _ystart = _colstart;
        _height = windowWidth;
        _width = windowHeight;
        break;
    }

    sendCommand(ST77XX_MADCTL, &madctl, 1);
}
