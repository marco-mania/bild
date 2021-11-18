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

#ifndef RLE_H
#define RLE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#include "types.h"

void rleEncode8(void *data, const int size, void *coded_data, int *coded_size, void *parameters);
void rleDecode8(void *coded_data, const int coded_size, void *data, int *size);

#endif
