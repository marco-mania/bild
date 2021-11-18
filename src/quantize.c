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

#include "quantize.h"

int32_t quantize(const int32_t element, const int quant_param)
{

  if (quant_param == 0) return element;

  if (element > 0)
    return element >> quant_param;
  else
    return -(ABS(element) >> quant_param);

}

int32_t dequantize(const int32_t element, const int quant_param)
{

  if (quant_param == 0) return element;

  return element << quant_param;

}
