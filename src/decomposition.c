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

#include "decomposition.h"

Level1D* Level1DCreate(const int l_size, Signal1D *h)
{

  Level1D *level = malloc(sizeof(Level1D));
  level->l_size = l_size;
  level->h = h;
  return level;

}

void Level1DDestroy(Level1D *level)
{

  Signal1DDestroy(level->h);
  free(level);

}

Levels1D* Levels1DCreate(const int level_count, const int size)
{

  Levels1D *levels = malloc(sizeof(Levels1D));
  levels->level_count = level_count;
  levels->levels = malloc(sizeof(Levels1D*)*level_count);
  levels->size = size;
  return levels;

}

void Levels1DDestroy(Levels1D *levels)
{

  int i;
  for (i = 0; i < levels->level_count; ++i)
    Level1DDestroy(levels->levels[i]);

  free(levels->levels);

  free(levels);

}

void DecomposeLevel1D(int32_t *source, const int source_size, Signal1D *l, Signal1D *h)
{

  const bool odd_size = source_size % 2;

  int i;
  for (i = 0; i < (source_size>>1); ++i)
  {
    HaarForwardTransform(source[i<<1], source[(i<<1)+1], &source[i<<1], &source[(i<<1)+1]);
    l->data[l->data_pos++] = source[i<<1];
    h->data[h->data_pos++] = source[(i<<1)+1];
  }

  if (odd_size)
    l->data[l->data_pos++] = source[source_size-1];

}

Levels1D* Decompose1D(Signal1D *signal0)
{

  Signal1D *signal = signal0;

  const int level_count = ilog2(get_next_pow(signal->size));

  Levels1D *levels = Levels1DCreate(level_count, signal->size);

  int s0, sb;
  int s1 = signal->size;

  bool odd_size;

  Level1D *level;
  int level_n = 0;
  while (level_n < level_count)
  {

    sb = s1;

    odd_size = sb % 2;

    s0 = sb >> 1;
    s1 = (sb+1) >> 1;

    level = Level1DCreate(s1, Signal1DCreate(odd_size?s1:s0));

    DecomposeLevel1D(signal->data, sb, signal, level->h);

    signal->data_pos = 0;
    level->h->data_pos = 0;

    levels->levels[level_n++] = level;

  }

  levels->root_value = signal->data[0];

  return levels;

}

void ReconstructLevel1D(int32_t *target, const int target_size, Signal1D *l, Signal1D *h)
{

  const bool odd_size = target_size % 2;

  int i;
  for (i = 0; i < (target_size>>1); ++i)
    HaarInverseTransform(l->data[l->data_pos++], h->data[h->data_pos++], &target[i<<1], &target[(i<<1)+1]);

  if (odd_size) target[target_size-1] = l->data[l->data_pos++];

}

Signal1D* reconstruct1D(Levels1D *levels)
{

  int level_n = levels->level_count-1;

  Signal1D *signal = Signal1DCreate(levels->size);

  Signal1D *l_src = Signal1DCreate(0);

  int32_t *l_trg;
  int l_trg_size;

  if (level_n % 2)
    signal->data[0] = levels->root_value;
  else
    signal->data[levels->size-1] = levels->root_value;

  while (level_n >= 0)
  {

    l_src->size = levels->levels[level_n]->l_size;
    l_src->data_pos = 0;

    l_trg_size = (level_n == 0)?levels->size:levels->levels[level_n-1]->l_size;

    if (level_n % 2)
    {
      l_trg = &signal->data[levels->size-l_trg_size];
      l_src->data = signal->data;
    }
    else
    {
      l_trg = signal->data;
      l_src->data = &signal->data[levels->size-l_src->size];
    }

    levels->levels[level_n]->h->data_pos = 0;

    ReconstructLevel1D(l_trg, l_trg_size, l_src, levels->levels[level_n]->h);

    --level_n;

  }

  signal->data_pos = 0;

  return signal;

}


Level2D* Level2DCreate(const int ll_width, const int ll_height, Signal2D *lh, Signal2D *hl, Signal2D *hh)
{

  Level2D *level = malloc(sizeof(Level2D));
  level->ll_width = ll_width;
  level->ll_height = ll_height;
  level->lh = lh;
  level->hl = hl;
  level->hh = hh;
  return level;

}

void Level2DDestroy(Level2D *level)
{

  Signal2DDestroy(level->lh);
  Signal2DDestroy(level->hl);
  Signal2DDestroy(level->hh);

  free(level);

}

Levels2D* Levels2DCreate(const int level_count, const int width, const int height)
{

  Levels2D *levels = malloc(sizeof(Levels2D));
  levels->level_count = level_count;
  levels->levels = malloc(sizeof(Level2D*)*level_count);
  levels->width = width;
  levels->height = height;
  return levels;

}

void Levels2DDestroy(Levels2D *levels)
{

  int i;
  for (i = 0; i < levels->level_count; ++i)
    Level2DDestroy(levels->levels[i]);

  free(levels->levels);

  free(levels);

}

void DecomposeLevel2D(int32_t *source, const int source_width, const int source_height, Signal2D *ll, Signal2D *lh, Signal2D *hl, Signal2D *hh, const int quant_param)
{

  const bool odd_width = source_width % 2;
  const bool odd_height = source_height % 2;

  int32_t *row0 = source;
  int32_t *row1 = source+source_width;

  int i, j;
  for (i = 0; i < (source_height>>1); ++i)
  {

    for (j = 0; j < (source_width>>1); ++j)
    {

      HaarForwardTransform(row0[j<<1], row0[(j<<1)+1], &row0[j<<1], &row0[(j<<1)+1]);
      HaarForwardTransform(row1[j<<1], row1[(j<<1)+1], &row1[j<<1], &row1[(j<<1)+1]);
      HaarForwardTransform(row0[j<<1], row1[j<<1], &row0[j<<1], &row1[j<<1]);
      HaarForwardTransform(row0[(j<<1)+1], row1[(j<<1)+1], &row0[(j<<1)+1], &row1[(j<<1)+1]);

      ll->data[ll->data_pos++] = row0[j<<1];
      hl->data[hl->data_pos++] = quantize(row1[j<<1], quant_param);
      lh->data[lh->data_pos++] = quantize(row0[(j<<1)+1], quant_param);
      hh->data[hh->data_pos++] = quantize(row1[(j<<1)+1], quant_param);

    }

    if (odd_width)
    {

      HaarForwardTransform(row0[source_width-1], row1[source_width-1], &row0[source_width-1], &row1[source_width-1]);
      ll->data[ll->data_pos++] = row0[source_width-1];
      hl->data[hl->data_pos++] = quantize(row1[source_width-1], quant_param);

    }

    row0 += (source_width << 1);
    row1 += (source_width << 1);

  }

  if (odd_height)
  {

    for (j = 0; j < (source_width>>1); ++j)
    {
      HaarForwardTransform(row0[j<<1], row0[(j<<1)+1], &row0[j<<1], &row0[(j<<1)+1]);
      ll->data[ll->data_pos++] = row0[j<<1];
      lh->data[lh->data_pos++] = quantize(row0[(j<<1)+1], quant_param);
    }

    if (odd_width)
      ll->data[ll->data_pos++] = row0[source_width-1];

  }

}

Levels2D* Decompose2D(Signal2D *signal0, const int quant_param)
{

  Signal2D *signal = signal0;

  const int level_count = ilog2(get_next_pow(MAX(signal->width, signal->height)));

  Levels2D *levels = Levels2DCreate(level_count, signal->width, signal->height);

  int w1 = signal->width;
  int h1 = signal->height;
  int w0, h0, wb, hb;

  bool odd_width, odd_height;

  int q = quant_param;

  Level2D *level;
  int level_n = 0;
  while (level_n < level_count)
  {

    wb = w1;
    hb = h1;

    odd_width = wb % 2;
    odd_height = hb % 2;

    w1 = (wb+1) >> 1;
    h1 = (hb+1) >> 1;
    w0 = wb >> 1;
    h0 = hb >> 1;

    level = Level2DCreate(w1, h1,
                          Signal2DCreate(w0, odd_height?h1:h0),
                          Signal2DCreate(odd_width?w1:w0, h0),
                          Signal2DCreate(w0, h0));

    DecomposeLevel2D(signal->data, wb, hb, signal, level->lh, level->hl, level->hh, q);

    signal->data_pos = 0;
    level->lh->data_pos = 0;
    level->hl->data_pos = 0;
    level->hh->data_pos = 0;

    levels->levels[level_n++] = level;

    if (q > 0) --q;

  }

  levels->root_value = signal->data[0];

  return levels;

}

void ReconstructLevel2D(int32_t *target, const int target_width, const int target_height, Signal2D *ll, Signal2D *lh, Signal2D *hl, Signal2D *hh, const int quant_param)
{

  const bool odd_width = target_width % 2;
  const bool odd_height = target_height % 2;

  int32_t *row0 = target;
  int32_t *row1 = target+target_width;

  int i, j;
  for (i = 0; i < (target_height>>1); ++i)
  {

    for (j = 0; j < (target_width>>1); ++j)
    {
      HaarInverseTransform(ll->data[ll->data_pos++], dequantize(hl->data[hl->data_pos++], quant_param), &row0[j<<1], &row1[j<<1]);
      HaarInverseTransform(dequantize(lh->data[lh->data_pos++], quant_param), dequantize(hh->data[hh->data_pos++], quant_param), &row0[(j<<1)+1], &row1[(j<<1)+1]);
      HaarInverseTransform(row0[j<<1], row0[(j<<1)+1], &row0[j<<1], &row0[(j<<1)+1]);
      HaarInverseTransform(row1[j<<1], row1[(j<<1)+1], &row1[j<<1], &row1[(j<<1)+1]);
    }

    if (odd_width)
      HaarInverseTransform(ll->data[ll->data_pos++], hl->data[hl->data_pos++], &row0[target_width-1], &row1[target_width-1]);

    row0 += (target_width << 1);
    row1 += (target_width << 1);

  }

  if (odd_height)
  {

    for (j = 0; j < (target_width>>1); ++j)
    {
      row0[j<<1] = ll->data[ll->data_pos++];
      row0[(j<<1)+1] = lh->data[lh->data_pos++];
      HaarInverseTransform(row0[j<<1], row0[(j<<1)+1], &row0[j<<1], &row0[(j<<1)+1]);
    }

    if (odd_width)
      row0[target_width-1] = ll->data[ll->data_pos++];

  }

}

Signal2D* Reconstruct2D(Levels2D *levels, const int quant_param)
{

  int level_n = levels->level_count-1;

  Signal2D *signal = Signal2DCreate(levels->width, levels->height);

  Signal2D *ll_src = Signal2DCreate(0, 0);

  int32_t *ll_trg;
  int ll_trg_width, ll_trg_height;

  if (level_n % 2)
    signal->data[0] = levels->root_value;
  else
    signal->data[(levels->width*levels->height)-1] = levels->root_value;

  int q = 0;

  while (level_n >= 0)
  {

    ll_src->width = levels->levels[level_n]->ll_width;
    ll_src->height = levels->levels[level_n]->ll_height;
    ll_src->data_pos = 0;

    ll_trg_width = (level_n == 0)?levels->width:levels->levels[level_n-1]->ll_width;
    ll_trg_height = (level_n == 0)?levels->height:levels->levels[level_n-1]->ll_height;

    if (level_n % 2)
    {
      ll_trg = &signal->data[(levels->width*levels->height)-(ll_trg_width*ll_trg_height)];
      ll_src->data = signal->data;
    }
    else
    {
      ll_trg = signal->data;
      ll_src->data = &signal->data[(levels->width*levels->height)-(ll_src->width*ll_src->height)];
    }

    levels->levels[level_n]->lh->data_pos = 0;
    levels->levels[level_n]->hl->data_pos = 0;
    levels->levels[level_n]->hh->data_pos = 0;

    ReconstructLevel2D(ll_trg, ll_trg_width, ll_trg_height, ll_src, levels->levels[level_n]->lh, levels->levels[level_n]->hl, levels->levels[level_n]->hh, q);

    --level_n;

    if ((level_n < quant_param) && (q < quant_param)) ++q;

  }

  signal->data_pos = 0;

  return signal;

}
