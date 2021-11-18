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

#include "rle.h"

void rleEncode8(void *data, const int size, void *coded_data, int *coded_size, void *parameters)
{

  byte *d = data;
  byte *cd = coded_data;

  byte b0 = d[0];
  byte b1;

  int coded_data_pos = 0;
  cd[coded_data_pos++] = b0;

  uint16_t l = 0;

  int i = 1;
  while (i < size)
  {

    b1 = d[i++];

    if (b1 == b0)
    {

      cd[coded_data_pos++] = b1;

      l = 0;
      while (((b1 = d[i++]) == b0) && (l < UINT16_MAX) && (i < size)) ++l;

      memcpy(&cd[coded_data_pos], &l, sizeof(uint16_t));
      coded_data_pos += sizeof(uint16_t);

    }

    cd[coded_data_pos++] = b1;

    b0 = b1;

  }

  *coded_size = coded_data_pos;

}

void rleDecode8(void *coded_data, const int coded_size, void *data, int *size)
{

  byte *cd = coded_data;
  byte *d = data;

  byte b0 = cd[0];
  byte b1;

  int data_pos = 0;
  d[data_pos++] = b0;

  uint16_t l;

  int i = 1;
  while (i < coded_size)
  {

    b1 = cd[i++];

    d[data_pos++] = b1;

    if (b1 == b0)
    {

      memcpy(&l, &cd[i], sizeof(uint16_t));
      i += sizeof(uint16_t);

      while (l > 0) { d[data_pos++] = b1; --l; }

      b1 = cd[i++];
      d[data_pos++] = b1;

    }

    b0 = b1;

  }

  *size = data_pos;

}
