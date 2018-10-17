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

#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "types.h"

#define MAX_NODE_COUNT 511

typedef struct tHuffmanNode HuffmanNode;

struct tHuffmanNode
{
  uint32_t frequency;
  byte symbol;
  uint32_t code;
  int code_length;
  HuffmanNode *parent, *left_child, *right_child;
};

void huffmanEncode(void *data, const int size, void *coded_data, int *coded_size, void *parameters);
void huffmanDecode(void *coded_data, const int coded_size, void *data, int *size);

#endif
