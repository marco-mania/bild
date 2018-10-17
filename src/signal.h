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

#ifndef SIGNAL_H
#define SIGNAL_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

struct tSignal1D
{
  int size;
  int32_t *data;
  int data_pos;
};
typedef struct tSignal1D Signal1D;

Signal1D* Signal1DCreate(const int size);
void Signal1DDestroy(Signal1D *signal);

void Signal1DAdd(Signal1D *signal, Signal1D *signal_sum);
void Signal1DSub(Signal1D *signal, Signal1D *signal_sum);




struct tSignal2D
{
  int width;
  int height;
  int32_t *data;
  int data_pos;
};
typedef struct tSignal2D Signal2D;

Signal2D* Signal2DCreate(const int width, const int height);
void Signal2DDestroy(Signal2D *signal);

/* Downsampling: Each sample_factor x sample_factor elements will be replaced by
 * their mean.
 * 1  2  3  4
 * 5  6  7  8
 * 9  10 11 12
 * 13 14 15 16
 * will be
 * 3  5
 * 11 13
 */
void Signal2DDownsample2(Signal2D *signal);
void Signal2DUpsample2(Signal2D *signal, const int target_width, const int target_height);

void Signal2DAdd(Signal2D *signal, Signal2D *signal_sum);
void Signal2DSub(Signal2D *signal, Signal2D *signal_sum);

#endif
