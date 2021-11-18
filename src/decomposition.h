/* BILD - Wavelet based image compression
 * All rights reserved (since 2004). Marco Nelles.
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

/* H L */

/* HH LH
 * HL HH */

#ifndef DECOMPOSITION_H
#define DECOMPOSITION_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"
#include "wavelet.h"
#include "signal.h"
#include "quantize.h"

struct tLevel1D
{
  int l_size;
  Signal1D *h;
};
typedef struct tLevel1D Level1D;

struct tLevels1D
{
  uint32_t root_value;
  Level1D **levels;
  int level_count;
  int size;
};
typedef struct tLevels1D Levels1D;

Level1D* Level1DCreate(const int l_size, Signal1D *h);
void Level1DDestroy(Level1D *level);

Levels1D* Levels1DCreate(const int level_count, const int size);
void Levels1DDestroy(Levels1D *levels);

void decomposeLevel1D(int32_t *source, const int source_size, Signal1D *l, Signal1D *h);
Levels1D* decompose1D(Signal1D *signal0);

/* Reconstruction, q = quality parameter */
void reconstructLevel1D(int32_t *target, const int target_size, Signal1D *l, Signal1D *h);
Signal1D* reconstruct1D(Levels1D *levels);




struct tLevel2D
{
  int ll_width;
  int ll_height;
  Signal2D *lh;
  Signal2D *hl;
  Signal2D *hh;
};
typedef struct tLevel2D Level2D;

struct tLevels2D
{
  uint32_t root_value;
  Level2D **levels;
  int level_count;
  int width;
  int height;
};
typedef struct tLevels2D Levels2D;

Level2D* Level2DCreate(const int ll_width, const int ll_height, Signal2D *lh, Signal2D *hl, Signal2D *hh);
void Level2DDestroy(Level2D *level);

Levels2D* Levels2DCreate(const int level_count, const int width, const int height);
void Levels2DDestroy(Levels2D *levels);

/* Mallat decomposition */
void DecomposeLevel2D(int32_t *source, const int source_width, const int source_height, Signal2D *ll, Signal2D *lh, Signal2D *hl, Signal2D *hh, const int quant_param);
Levels2D* Decompose2D(Signal2D *signal0, const int quant_param);

/* Mallat reconstruction */
void ReconstructLevel2D(int32_t *target, const int target_width, const int target_height, Signal2D *ll, Signal2D *lh, Signal2D *hl, Signal2D *hh, const int quant_param);
Signal2D* Reconstruct2D(Levels2D *levels, const int quant_param);

#endif
