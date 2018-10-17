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

#include <FreeImage.h>

#include "types.h"

#include "signal.h"

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

Image* ImageLoadFromBMPFileAndCreate(const char *filename);
void ImageSaveAsBMPFile(Image *image, const char *filename);

void ImageTransformColourSpace(Image *image, const ColourSpace new_cs);

#endif
