/* BILD - Wavelet based image compression
 * All rights reserved (2014-2018). Marco Nelles.
 * <https://gitlab.com/wavelets/bild>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef IMAGE_H
#define IMAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#include "signal.h"

#define BMP_MAGIC 19778

#pragma pack(push, 1)

struct tBMPFileHeader
{
  uint16_t bfType;      /* must always be set to 'BM' (int: 19778) to declare that this is a .bmp-file. */
  uint32_t bfSize;      /* specifies the size of the file in bytes. */
  uint32_t bfReserved;  /* must always be set to zero. */
  uint32_t bfOffBits;   /* specifies the offset from the beginning of the file to the bitmap data. */
};

struct tBMPInfoHeader
{
  uint32_t biSize;         /* specifies the size (std: 40) of the BMPInfoHeader structure, in bytes. */
  int32_t biWidth;         /* specifies the width of the image, in pixels. */
  int32_t biHeight;        /* specifies the height of the image, in pixels. */
  uint16_t biPlanes;       /* specifies the number of planes of the target device, must be set to zero. */
  uint16_t biBitCount;     /* specifies the number of bits per pixel. */
  uint32_t biCompression;  /* Specifies the type of compression, usually set to zero (no compression). */
  uint32_t biSizeImage;    /* specifies the size of the image data, in bytes. If there is no compression, it is valid to set this member to zero. */
  int32_t biXPelsPerMeter; /* specifies the the horizontal pixels per meter on the designated targer device, usually set to zero. */
  int32_t biYPelsPerMeter; /* specifies the the vertical pixels per meter on the designated targer device, usually set to zero. */
  uint32_t biClrUsed;      /* specifies the number of colors used in the bitmap, if set to zero the number of colors is calculated using the biBitCount member. */
  uint32_t biClrImportant; /* specifies the number of color that are 'important' for the bitmap, if set to zero, all colors are important. */
};

#pragma pack(pop)

typedef struct tBMPInfoHeader BMPInfoHeader;
typedef struct tBMPFileHeader BMPFileHeader;

struct tBMP24BitPixel
{
  uint8_t blue;
  uint8_t green;
  uint8_t red;
};
typedef struct tBMP24BitPixel BMP24BitPixel;

enum tColourSpace { Grayscale = 0, RGB, YCbCr411 };
typedef enum tColourSpace ColourSpace;

struct tImage
{
  ColourSpace colour_space;
  int width;
  int height;
  Signal2D **channels;
  int channel_count;
};
typedef struct tImage Image;

Image* ImageCreate(const int width, const int height, const ColourSpace cs);
void ImageDestroy(Image *image);

Image* ImageLoadFromBMPFileAndCreate_freetype(const char *filename);
void ImageSaveAsBMPFile_freetype(Image *image, const char *filename);

Image* ImageLoadFromBMPFileAndCreate(const char *filename);
void ImageSaveAsBMPFile(Image *image, const char *filename);

void ImageTransformColourSpace(Image *image, const ColourSpace new_cs);

#endif
