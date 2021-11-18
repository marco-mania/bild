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

#ifndef WAVELET_H
#define WAVELET_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

void HaarForwardTransform(const int32_t s1, const int32_t s2, int32_t *s, int32_t *d);
void HaarInverseTransform(const int32_t s, const int32_t d, int32_t *s1, int32_t *s2);

#endif
