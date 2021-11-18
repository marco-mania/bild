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

#include "pack.h"

const int8_t pack32_8(const int32_t i32, int32_t *overflow_buf, int *overflow_buf_pos)
{

  if (ABS(i32) > 127)
  {
    overflow_buf[(*overflow_buf_pos)++] = i32;
    return -128;
  }

  return (int8_t)i32;

}

const int32_t unpack8_32(const int8_t i8, int32_t *overflow_buf, int *overflow_buf_pos)
{

  if (i8 == -128) return overflow_buf[(*overflow_buf_pos)++];

  return (int32_t)i8;

}
