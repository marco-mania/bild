/* BILD - Wavelet based image compression
 * All rights reserved (since 2004). Marco Nelles.
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

#ifndef BILD_H
#define BILD_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>

#include "globals.h"
#include "types.h"
#include "image.h"
#include "decomposition.h"
#include "pack.h"
#include "rle.h"
#include "huffman.h"

#define BILD_TYPE         0x444C4942

#pragma pack(push, 1)

struct tBILDHeader
{
  uint32_t type;            /* Magic */
  uint16_t version;         /* Version */
  uint32_t width;           /* Width of the image in pixels */
  uint32_t height;          /* Height of the image in pixels */
  uint32_t quality;         /* Quality parameter */
};

struct tBILDLevelsHeader
{
  uint32_t root_value;
  uint32_t level_count;
  uint32_t width;
  uint32_t height;
  uint32_t coded_size;      /* Size of the coded levels in bytes */
  uint32_t overflow_buffer_size; /* Size of overflow buffer */
};

struct tBILDLevelHeader
{
  uint32_t lh_width;
  uint32_t lh_height;
  uint32_t hl_width;
  uint32_t hl_height;
  uint32_t hh_width;
  uint32_t hh_height;
  uint32_t ll_width;
  uint32_t ll_height;
};

#pragma pack(pop)

typedef struct tBILDHeader BILDHeader;
typedef struct tBILDLevelsHeader BILDLevelsHeader;
typedef struct tBILDLevelHeader BILDLevelHeader;

Image *ImageLoadFromBILDFileAndCreate(const char *filename);
void ImageSaveAsBILDFile(Image *image, const char *filename, const int quality);

void BILDPrintInformation(const char *filename);

#endif
