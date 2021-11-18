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

#ifndef TYPES_H
#define TYPES_H

#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef int bool;
#define true 1
#define false 0

#define CLIP(X) ((X) > 255 ? 255 : (X) < 0 ? 0 : X)
#define ABS(X) (X > 0 ? X : -X)
#define MAX(A, B) (((A) > (B)) ? (A) : (B))
#define SGN(X) ((X > 0) - (X < 0))

typedef unsigned char byte;

#define BYTE_MAX 255

static inline uint32_t get_next_pow(const uint32_t i)
{
  uint32_t n = i > 0 ? i - 1 : 0;
  n |= n >> 1;
  n |= n >> 2;
  n |= n >> 4;
  n |= n >> 8;
  n |= n >> 16;
  ++n;
  return n;
}

static inline uint32_t get_prev_pow(uint32_t i)
{
  uint32_t n = i;
  n = n | (n >> 1);
  n = n | (n >> 2);
  n = n | (n >> 4);
  n = n | (n >> 8);
  n = n | (n >> 16);
  return n - (n >> 1);
}

static inline uint32_t ilog2(uint32_t val)
{
  int a;
  __asm__
  ("\
    xorl  %0, %0      ;\
    decl  %0       ;\
    bsrl  %1, %0      ;\
   "
   : "=&r" (a)
   : "mr" (val)
   : "cc"
  );
  return a;
}

#endif
