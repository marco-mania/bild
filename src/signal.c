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

#include "signal.h"

Signal1D* Signal1DCreate(const int size)
{

  Signal1D *signal = malloc(sizeof(Signal1D));
  signal->size = size;
  signal->data_pos = 0;

  signal->data = malloc(signal->size*sizeof(int32_t));

  return signal;

}

void Signal1DDestroy(Signal1D *signal)
{

  free(signal->data);

  free(signal);

}

void Signal1DAdd(Signal1D *signal, Signal1D *signal_sum)
{

  int i;
  for (i = 0; i < signal->size; ++i)
    signal->data[i] += signal_sum->data[i];

}

void Signal1DSub(Signal1D *signal, Signal1D *signal_sum)
{

  int i;
  for (i = 0; i < signal->size; ++i)
    signal->data[i] -= signal_sum->data[i];

}




Signal2D* Signal2DCreate(const int width, const int height)
{

  Signal2D *signal = malloc(sizeof(Signal2D));
  signal->width = width;
  signal->height = height;
  signal->data_pos = 0;

  signal->data = malloc(signal->width*signal->height*sizeof(int32_t));

  return signal;

}

void Signal2DDestroy(Signal2D *signal)
{

  free(signal->data);

  free(signal);

}

void Signal2DDownsample2(Signal2D *signal)
{

  const int width = signal->width >> 1; /* /2 */
  const int height = signal->height >> 1; /* /2 */

  const bool odd_width = signal->width % 2;
  const bool odd_height = signal->height % 2;

  int32_t *row0 = signal->data;
  int32_t *row1 = signal->data+signal->width;

  int32_t *row = signal->data;

  int i, j;
  for (j = 0; j < height; ++j)
  {

    for (i = 0; i < width; ++i)
    {
      *row = (row0[0] + row0[1] + row1[0] + row1[1]) >> 2;
      ++row; row0 += 2; row1 += 2;
    }

    if (odd_width)
    {
      *row = (row0[0] + row1[0]) >> 1;
      ++row; ++row0; ++row1;
    }

    row0 += signal->width;
    row1 += signal->width;

  }

  if (odd_height)
  {
    for (i = 0; i < width; ++i)
    {
      *row = (row0[0] + row0[1]) >> 1;
      ++row; row0 += 2;
    }
    if (odd_width) *row = row0[0];
  }

  signal->width = (signal->width+1) >> 1;
  signal->height = (signal->height+1) >> 1;
  signal->data = (int32_t*)realloc(signal->data, signal->width * signal->height * sizeof(int32_t));

}

void Signal2DUpsample2(Signal2D *signal, const int target_width, const int target_height)
{

  const bool odd_width = target_width % 2;
  const bool odd_height = target_height % 2;

  Signal2D *result = Signal2DCreate(target_width, target_height);

  int32_t *row0 = result->data;
  int32_t *row1 = result->data+result->width;

  int32_t *row = signal->data;

  int i, j;
  for (j = 0; j < result->height>>1; ++j)
  {

    for (i = 0; i < result->width>>1; ++i)
    {
      row0[i<<1] = *row;
      row1[i<<1] = *row;
      row0[(i<<1)+1] = *row;
      row1[(i<<1)+1] = *row;
      ++row;
    }

    if (odd_width)
    {
      row0[i<<1] = *row;
      row1[i<<1] = *row;
      ++row;
    }

    row0 += result->width << 1;
    row1 += result->width << 1;

  }

  if (odd_height)
  {
    for (i = 0; i < (result->width>>1); ++i)
    {
      row0[i<<1] = *row;
      row0[(i<<1)+1] = *row;
      ++row;
    }
    if (odd_width) row0[i<<1] = *row;
  }

  free(signal->data);
  signal->width = result->width;
  signal->height = result->height;
  signal->data = result->data;

  free(result);

}

void Signal2DAdd(Signal2D *signal, Signal2D *signal_sum)
{

  int i;
  for (i = 0; i < signal->width*signal->height; ++i)
    signal->data[i] += signal_sum->data[i];

}

void Signal2DSub(Signal2D *signal, Signal2D *signal_sum)
{

  int i;
  for (i = 0; i < signal->width*signal->height; ++i)
    signal->data[i] -= signal_sum->data[i];

}
