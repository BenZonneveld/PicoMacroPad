/*!
 * @file ImageReader.h
 *
 * This is part of Adafruit's ImageReader library for Arduino, designed to
 * work with Adafruit_GFX plus a display device-specific library.
 *
 * Adafruit invests time and resources providing this open source code,
 * please support Adafruit and open-source hardware by purchasing
 * products from Adafruit!
 *
 * Written by Phil "PaintYourDragon" Burgess for Adafruit Industries.
 *
 * BSD license, all text here must be included in any redistribution.
 */
#ifndef __Image_READER_H__
#define __Image_READER_H__

#include "MMC_SD.h"
#include "Adafruit_SPITFT.h"

/** Status codes returned by drawBMP() and loadBMP() */
enum ImageReturnCode {
  IMAGE_SUCCESS,            // Successful load (or image clipped off screen)
  IMAGE_ERR_FILE_NOT_FOUND, // Could not open file
  IMAGE_ERR_FORMAT,         // Not a supported image format
  IMAGE_ERR_MALLOC          // Could not allocate image (loadBMP() only)
};

/** Image formats returned by loadBMP() */
enum ImageFormat {
  IMAGE_NONE, // No image was loaded; IMAGE_ERR_* condition
  IMAGE_1,    // GFXcanvas1 image (NOT YET SUPPORTED)
  IMAGE_8,    // GFXcanvas8 image (NOT YET SUPPORTED)
  IMAGE_16    // GFXcanvas16 image (SUPPORTED)
};

/*!
   @brief  Data bundle returned with an image loaded to RAM. Used by
           ImageReader.loadBMP() and Image.draw(), not ImageReader.drawBMP().
*/
class Image {
public:
  Image(void);
  ~Image(void);
  int16_t width(void) const;  // Return image width in pixels
  int16_t height(void) const; // Return image height in pixels
  void draw(Adafruit_SPITFT &tft, int16_t x, int16_t y);
  ImageFormat getFormat(void) const { return (ImageFormat)format; }
  void *getCanvas(void) const;
  uint16_t *getPalette(void) const { return palette; }
  GFXcanvas1 *getMask(void) const { return mask; };

protected:
  union {                  // Single pointer, only one variant is used:
    GFXcanvas1 *canvas1;   ///< Canvas object if 1bpp format
    GFXcanvas8 *canvas8;   ///< Canvas object if 8bpp format
    GFXcanvas16 *canvas16; ///< Canvas object if 16bpp
  } canvas;                ///< Union of different GFXcanvas types
  GFXcanvas1 *mask;        ///< 1bpp image mask (or NULL)
  uint16_t *palette;       ///< Color palette for 8bpp image (or NULL)
  uint8_t format;          ///< Canvas bundle type in use
  void dealloc(void);      ///< Free/deinitialize variables
  friend class ImageReader; ///< Loading occurs here
};

class ImageReader{
public:
    ImageReader() {};
  ImageReader(MMC_SD &mmc);
  ~ImageReader(void);
  ImageReturnCode drawBMP(const char *filename, Adafruit_SPITFT &tft, int16_t x,
                          int16_t y);
  ImageReturnCode loadBMP(const char *filename, Image &img);
  ImageReturnCode bmpDimensions(const char *filename, int32_t *w, int32_t *h);
  const char * printStatus(ImageReturnCode stat);
  uint8_t Dirs(char* DirName);
protected:
  MMC_SD *filesys; ///< FAT FileSystem Object
  FIL file;              ///< Current Open file
  ImageReturnCode coreBMP(const char *filename, Adafruit_SPITFT *tft,
                          uint16_t *dest, int16_t x, int16_t y,
                          Image *img);
  uint16_t readLE16(void);
  uint32_t readLE32(void);
};

#endif // __Image_READER_H__
