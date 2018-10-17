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

#include "image.h"

Image* ImageCreate(const int width, const int height, const ColourSpace cs)
{

  Image *image = malloc(sizeof(Image));
  image->width = width;
  image->height = height;
  image->colour_space = cs;

  switch (image->colour_space) {
    case Grayscale : image->channel_count = 1; break;
    case RGB :
    case YCbCr411 : image->channel_count = 3; break;
  }

  image->channels = malloc(sizeof(Signal2D*)*image->channel_count);

  int i;
  if ((width == 0) || (height == 0))
  {
    for (i = 0; i < image->channel_count; ++i) image->channels[i] = NULL;
  }
  else
  {
    for (i = 0; i < image->channel_count; ++i)
      image->channels[i] = Signal2DCreate(image->width, image->height);
  }

  return image;

}

void ImageDestroy(Image *image) {

  int i;
  for (i = 0; i < image->channel_count; ++i)
    Signal2DDestroy(image->channels[i]);

  free(image->channels);

  free(image);

}

Image* ImageLoadFromBMPFileAndCreate(const char *filename) {

  FreeImage_Initialise(FALSE);

  FIBITMAP *bmp = FreeImage_Load(FIF_BMP, filename, BMP_DEFAULT);

  /*const int bpp = FreeImage_GetBPP(bmp);*/
  const int w = FreeImage_GetWidth(bmp);
  const int h = FreeImage_GetHeight(bmp);

  Image *result = ImageCreate(w, h, RGB);

  int x, y; byte *data;
  for (y = 0; y < h; ++y)
  {
    data = FreeImage_GetScanLine(bmp, h-y-1);
    for (x = 0; x < w; ++x) {
      result->channels[0]->data[result->channels[0]->data_pos++] = data[FI_RGBA_RED];
      result->channels[1]->data[result->channels[1]->data_pos++] = data[FI_RGBA_GREEN];
      result->channels[2]->data[result->channels[2]->data_pos++] = data[FI_RGBA_BLUE];
      data += 3;
    }
  }

  result->channels[0]->data_pos = 0;
  result->channels[1]->data_pos = 0;
  result->channels[2]->data_pos = 0;

  FreeImage_Unload(bmp);

  FreeImage_DeInitialise();

  return result;

}

void ImageSaveAsBMPFile(Image *image, const char *filename) {

  if (image->colour_space != RGB) return;

  FreeImage_Initialise(FALSE);

  const int bpp = 24;
  const int w = image->width;
  const int h = image->height;

  FIBITMAP *bmp = FreeImage_Allocate(w, h, bpp, FI_RGBA_RED_MASK, FI_RGBA_GREEN_MASK, FI_RGBA_BLUE_MASK);

  image->channels[0]->data_pos = 0;
  image->channels[1]->data_pos = 0;
  image->channels[2]->data_pos = 0;

  int x, y; byte *data;
  for (y = 0; y < h; ++y)
  {
    data = FreeImage_GetScanLine(bmp, h-y-1);
    for (x = 0; x < w; ++x)
    {
      data[FI_RGBA_RED] = CLIP(image->channels[0]->data[image->channels[0]->data_pos]);
      data[FI_RGBA_GREEN] = CLIP(image->channels[1]->data[image->channels[1]->data_pos]);
      data[FI_RGBA_BLUE] = CLIP(image->channels[2]->data[image->channels[2]->data_pos]);
      ++image->channels[0]->data_pos;
      ++image->channels[1]->data_pos;
      ++image->channels[2]->data_pos;
      data += 3;
    }
  }

  FreeImage_Save(FIF_BMP, bmp, filename, 0);

  FreeImage_Unload(bmp);

  FreeImage_DeInitialise();

}

void ImageTransformColourSpace(Image *image, const ColourSpace new_cs)
{

  if (image->colour_space == new_cs) return;

  int i;

  if ((image->colour_space == YCbCr411) && (new_cs == RGB)) /* YCbCr411 -> RGB */
  {

    Signal2DUpsample2(image->channels[1], image->width, image->height);
    Signal2DUpsample2(image->channels[2], image->width, image->height);

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t Y = image->channels[0]->data[i];
      const int32_t Cb = image->channels[1]->data[i];
      const int32_t Cr = image->channels[2]->data[i];
      const int32_t G = Y - ((Cb + Cr) >> 2) + 128;

      image->channels[0]->data[i] = Cb + G;
      image->channels[1]->data[i] = G;
      image->channels[2]->data[i] = Cr + G;

    }

  }
  else if ((image->colour_space == RGB) && (new_cs == YCbCr411)) /* RGB -> YCbCr411 */
  {

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t R = image->channels[0]->data[i];
      const int32_t G = image->channels[1]->data[i];
      const int32_t B = image->channels[2]->data[i];

      image->channels[0]->data[i] = ((R + (G<<1) + B) >> 2) - 128;
      image->channels[1]->data[i] = R - G;
      image->channels[2]->data[i] = B - G;

    }

    Signal2DDownsample2(image->channels[1]);
    Signal2DDownsample2(image->channels[2]);

  }
  else if ((image->colour_space == RGB) && (new_cs == Grayscale)) /* RGB -> Grayscale */
  {

    for (i = 0; i < image->width*image->height; ++i)
    {

      const int32_t R = image->channels[0]->data[i];
      const int32_t G = image->channels[1]->data[i];
      const int32_t B = image->channels[2]->data[i];

      image->channels[0]->data[i] = (R * 11 + G * 16 + B * 5) >> 5;

    }

    free(image->channels[1]);
    image->channels[1] = NULL;
    free(image->channels[2]);
    image->channels[2] = NULL;
    image->channel_count = 1;

  }
  else if ((image->colour_space == YCbCr411) && (new_cs == Grayscale)) /* YCbCr411 -> Grayscale */
  {

    free(image->channels[1]);
    image->channels[1] = NULL;
    free(image->channels[2]);
    image->channels[2] = NULL;
    image->channel_count = 1;

  }

  image->colour_space = new_cs;

}
