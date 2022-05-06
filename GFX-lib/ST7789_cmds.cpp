/**************************************************************************
  Written by Limor Fried/Ladyada for Adafruit Industries.
  MIT license, all text above must be included in any redistribution
 **************************************************************************/

#include "ST7789_cmds.h"

static const uint8_t generic_st7789[] = {                // Init commands for 7789 screens
    9,                              //  9 commands in list:
    ST77XX_SWRESET,   ST_CMD_DELAY, //  1: Software reset, no args, w/delay
      100,                          //     ~150 ms delay
    ST77XX_SLPOUT ,   ST_CMD_DELAY, //  2: Out of sleep mode, no args, w/delay
      10,                          //      10 ms delay
    ST77XX_COLMOD , 1 + ST_CMD_DELAY, //  3: Set color mode, 1 arg + delay:
      0x55,                         //     16-bit color
      10,                           //     10 ms delay
    ST77XX_MADCTL , 1,              //  4: Mem access ctrl (directions), 1 arg:
      0x08,                         //     Row/col addr, bottom-top refresh
    ST77XX_CASET  , 4,              //  5: Column addr set, 4 args, no delay:
      0x00,
      0,        //     XSTART = 0
      0,
      240,  //     XEND = 240
    ST77XX_RASET  , 4,              //  6: Row addr set, 4 args, no delay:
      0x00,
      0,             //     YSTART = 0
      320 >> 8,
      320 & 0xFF,  //     YEND = 320
    ST77XX_INVON  ,   ST_CMD_DELAY,  //  7: hack
      10,
    ST77XX_NORON  ,   ST_CMD_DELAY, //  8: Normal display on, no args, w/delay
      10,                           //     10 ms delay
    ST77XX_DISPON ,   ST_CMD_DELAY, //  9: Main screen turn on, no args, delay
      10 };                          //    10 ms delay

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
